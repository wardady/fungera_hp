// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <filesystem>
#include <boost/archive/xml_oarchive.hpp>

#include "Fungera.h"
#include "mainwindow.h"

#include <QApplication>

namespace po = boost::program_options;
namespace fs = std::filesystem;

constexpr char xml_snapshots_dir[] = "xml_snapshots";

void convert_snapshots(const std::string &path) {
    if (!fs::exists(path))
        throw fs::filesystem_error(
                "Path to snapshot or directory with snapshots is not valid!",
                std::error_code{});

    std::vector<std::string> snapshots;
    fs::create_directories(xml_snapshots_dir);
    if (fs::is_directory(path)) {
        for (const auto &entry : fs::directory_iterator(path))
            if (entry.path().extension() == ".txt")
                snapshots.emplace_back(entry.path());
    } else {
        if (path.ends_with(".txt"))
            snapshots.emplace_back(path);
    }
    Fungera simulation{};
    for (const auto &snapshot:snapshots) {
        simulation.load_from_snapshot(snapshot);
        std::cout << "[LOG]: Converting snapshot " << snapshot << " ..."
                  << std::endl;
        std::stringstream snapshot_file{};
        snapshot_file << xml_snapshots_dir << fs::path::preferred_separator
                      << fs::path(snapshot).stem().string()
                      << ".xml";
        std::ofstream ofs(snapshot_file.str(), std::ios::trunc);
        assert(ofs.good());
        boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("Fungera", simulation);
    }
}

int main(int argc, char *argv[]) {
    po::options_description options("Options");
    options.add_options()
            ("help,h", "print help message")
            ("gui,g", po::value<bool>()->default_value(true),
             "execute with GUI or with basic logging")
            ("convert,c",
             po::value<std::string>()->default_value("")->implicit_value(
                     "./snapshots"), "convert snapshots to XML")
            ("load,l", po::value<std::string>(), "load from snapshot");
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    std::string convert_xml_dir = vm["convert"].as<std::string>();
    if (!convert_xml_dir.empty()) {
        std::cout << "Converting snapshots in " << convert_xml_dir
                  << " into XML format..." << std::endl;
        convert_snapshots(convert_xml_dir);
        return 0;
    }

    Fungera simulation{"config.toml"};
    if (vm.count("load")) {
        simulation.load_from_snapshot(vm["load"].as<std::string>());
    }

    if (vm["gui"].as<bool>()) {
        std::cout << "Starting simulation with GUI" << std::endl;
        QApplication a(argc, argv);
        MainWindow w{&simulation};
        w.show();
        return a.exec();
    } else {
        simulation.run();
    }

//    Fungera a{};
//    a.load_from_snapshot();
//    a.run();

    return 0;
}
