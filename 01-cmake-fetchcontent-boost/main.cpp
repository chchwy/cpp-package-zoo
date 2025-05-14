#include <boost/program_options.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(), "input file")
        ("output,o", po::value<std::string>(), "output file");

    // Parse command line arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Display help message if requested
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    // Check for required options
    if (!vm.count("input")) {
        std::cerr << "Input file is required.\n";
        return 1;
    }

    if (!vm.count("output")) {
        std::cerr << "Output file is required.\n";
        return 1;
    }

    // Process input and output files
    std::string input_file = vm["input"].as<std::string>();
    std::string output_file = vm["output"].as<std::string>();

    std::cout << "Input file: " << input_file << "\n";
    std::cout << "Output file: " << output_file << "\n";

    return 0;
}