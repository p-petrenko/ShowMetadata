#include "ShowMetadataConfig.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <stdio.h>
#include <iostream>
#include <optional>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

po::options_description createOptions() {
    // declaring all allowed options
    po::options_description desc("Allowed options");
    // declare oprions
    desc.add_options()
        ("help", "produce help message")
        ("version", "produce version number")
        ("melody_path", po::value<std::string>(), "set a path to a wav file")
        ;
    return desc;
}

po::variables_map createVarToStore(int argc, const char *argv[], const po::options_description& desc) {
    // declare a variable to store
    po::variables_map vm;
    // save results to variable_map
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    return vm;
}

std::optional<std::string> path(const std::string& melody_path_key, po::variables_map& vm) {
    return vm.count(melody_path_key)
               ? std::optional<std::string>{vm[melody_path_key].as<std::string>()}
               : std::nullopt;
}

int printMetadata(const std::string& melody_path) {
    AVFormatContext *fmt_ctx = NULL;
    const AVDictionaryEntry *tag = NULL;
    int ret;

    std::cout << "melody path was set to " << melody_path << "\n";

    if ((ret = avformat_open_input(&fmt_ctx, (melody_path).c_str(), NULL, NULL))) {
        std::cout << "Error in avformat_open_input\n";
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    while ((tag = av_dict_iterate(fmt_ctx->metadata, tag)))
        printf("%s=%s\n", tag->key, tag->value);

    avformat_close_input(&fmt_ctx);
    return 0;
}

int main (int argc, const char *argv[])
{
    try {
        auto desc = createOptions();
        auto vm = createVarToStore(argc, argv, desc);
        const auto version_key = "version";
        const auto help_key = "help";
        const auto melody_path_key = "melody_path";

        // version
        if (vm.count(version_key)) {
            std::cout << "Version of this app is " << SHOW_METADATA_VERSION << ".\n";
            return EXIT_SUCCESS;
        }

        // help
        if (vm.count(help_key)) {
            std::cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        // melody_path
        auto melody_path = path(melody_path_key, vm);
        if (melody_path) {
            return printMetadata(*melody_path);
        } else {
            std::cout << "melody path was not set\n";
            return EXIT_FAILURE;
        }
    } catch(std::exception& err) {
        std::cerr << "Error: " << err.what() << "\n";
        return EXIT_FAILURE;
    } catch(...) {
        std::cerr << "Exception of unknown type!\n";
        return EXIT_FAILURE;
    }
}
