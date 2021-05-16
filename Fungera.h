#ifndef FUNGERA_FUNGERA_H
#define FUNGERA_FUNGERA_H

#include <string>
#include <random>
#include <QObject>
#include <QString>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include "Memory.h"
#include "Queue.h"
#include "Organism.h"
#include "Config.h"
#include <fstream>

namespace mp = boost::multiprecision;

class Fungera : public QObject {
Q_OBJECT

    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive &ar, unsigned int version) const {
        ar & boost::serialization::make_nvp("cycle", cycle);
        ar & BOOST_SERIALIZATION_NVP(config);
        ar & BOOST_SERIALIZATION_NVP(memory);
        ar & BOOST_SERIALIZATION_NVP(queue);
        ar & BOOST_SERIALIZATION_NVP(purges);
        std::stringstream distribution_saver{};
        distribution_saver << radiation_dist;
        auto distribution_probabilities = distribution_saver.str();
        ar & BOOST_SERIALIZATION_NVP(distribution_probabilities);
    }

    template<class Archive>
    void load(Archive &ar, unsigned int version) {
        ar & cycle;
        ar & config;
        // TODO: possible ro run with new config (memory_ptr_m size/
        //  random (smth more???) seed must be the same)

        ar & memory;
        ar & queue;
        queue.kill_organisms_ratio = config.kill_organisms_ratio;
        for (auto &organism: queue.get_container()) {
            organism.conf_ptr_m = &config;
            organism.memory_ptr_m = &memory;
            organism.organism_queue_ptr_m = &queue;
        }

        ar & purges;
        geneaology_log.open("genealogy_log.log");
        std::stringstream distribution_saver{};
        std::string distribution_probabilities{};
        ar & distribution_probabilities;
        distribution_saver.str(distribution_probabilities);
        distribution_saver >> radiation_dist;
        is_running_m = false;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
public:
    Config config;

    Memory memory;

    //! TODO: Implement organisms enumeration/iteration on their list.
    Queue queue;

    std::atomic<int> is_running_m;

    Fungera();

    Fungera(const Fungera &) = delete;

    Fungera &operator=(const Fungera &) = delete;

    explicit Fungera(const std::string &config_path);

    ~Fungera() = default;

    void load_from_snapshot(const std::string &path);

    const std::optional<std::reference_wrapper<Organism>>
    get_organism(size_t organism_id);

    void execute_cycle();

    auto get_organisms_num() const { return queue.size(); }

    int is_running() const { return is_running_m; }

private:
    mp::checked_uint1024_t cycle;
    size_t purges = 0;
    std::discrete_distribution<int> radiation_dist;
    std::ofstream geneaology_log;

    void load_initial_genome(const std::string &filename,
                             const std::array<size_t, 2> &address);

    void radiation();

    void info_log();

    void new_child_log();

    void save_snapshot();

public slots:

    void run();

    void toggle_simulaiton();

signals:

    void cycle_changed(QString cycle);

    void alive_changed(quint64 num_alive);

    void purges_changed(quint64 num_purges);

    void alive_ids(QVector<size_t> organisms);
};

template<typename T>
QString reg_to_QString(const std::array<T, 2> &reg_val) {
    return QString("[%1, %2]").arg(reg_val[0]).arg(reg_val[1]);
}

#endif //FUNGERA_FUNGERA_H
