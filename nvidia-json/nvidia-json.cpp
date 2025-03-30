/*
 *  nvidia-json - Parses the NVIDIA supported-gpus.json file and
 *                generates the nvidia-detect.h header file
 *
 *  Copyright (C) 2025 Tuan Hoang <tqhoang@elrepo.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "json/json.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

#include <getopt.h>
extern char *optarg;
extern int optind, opterr, optopt;


// line type prefixes
const string ONE_TAB("\t");
const string ONE_SPACE(" ");


// legacy branch enums
typedef enum
{
    LEGACYBRANCH_FALSE,
    LEGACYBRANCH_71XX,
    LEGACYBRANCH_96XX,
    LEGACYBRANCH_173XX,
    LEGACYBRANCH_304XX,
    LEGACYBRANCH_340XX,
    LEGACYBRANCH_367XX,
    LEGACYBRANCH_390XX,
    LEGACYBRANCH_470XX,
    LEGACYBRANCH_UNKNOWN
} legacybranch_t;


// current branch enums
typedef enum
{
    KERNELOPEN_FALSE,
    KERNELOPEN_TRUE
} kernelopen_t;


// device info structure
typedef struct
{
    string devid;
    string subdevid;
    string name;
    legacybranch_t legacybranch;
    kernelopen_t kernelopen;
} device_info_t;


// maps <devid, device_info>
typedef map<string, device_info_t> devices_map_t;


// globals
devices_map_t devices_map;
devices_map_t::iterator devices_map_iter;


// legacy branch arrays and mappings
const string LEGACYBRANCH_FALSE_VERSION   = "";
const string LEGACYBRANCH_71XX_VERSION    = "71.86.xx";
const string LEGACYBRANCH_96XX_VERSION    = "96.43.xx";
const string LEGACYBRANCH_173XX_VERSION   = "173.14.xx";
const string LEGACYBRANCH_304XX_VERSION   = "304.xx";
const string LEGACYBRANCH_340XX_VERSION   = "340.xx";
const string LEGACYBRANCH_367XX_VERSION   = "367.xx";
const string LEGACYBRANCH_390XX_VERSION   = "390.xx";
const string LEGACYBRANCH_470XX_VERSION   = "470.xx";
const string LEGACYBRANCH_UNKNOWN_VERSION = "UNKNOWN";

const string LEGACYBRANCH_FALSE_ARRAY   = "";
const string LEGACYBRANCH_71XX_ARRAY    = "nv_71xx_pci_ids[]";
const string LEGACYBRANCH_96XX_ARRAY    = "nv_96xx_pci_ids[]";
const string LEGACYBRANCH_173XX_ARRAY   = "nv_173xx_pci_ids[]";
const string LEGACYBRANCH_304XX_ARRAY   = "nv_304xx_pci_ids[]";
const string LEGACYBRANCH_340XX_ARRAY   = "nv_340xx_pci_ids[]";
const string LEGACYBRANCH_367XX_ARRAY   = "nv_367xx_pci_ids[]";
const string LEGACYBRANCH_390XX_ARRAY   = "nv_390xx_pci_ids[]";
const string LEGACYBRANCH_470XX_ARRAY   = "nv_470xx_pci_ids[]";
const string LEGACYBRANCH_UNKNOWN_ARRAY = "nv_unknown_pci_ids[]";

bool create_legacybranch_ver2enum_map(map<legacybranch_t, string> &m1, map<string, legacybranch_t> &m2)
{
    m1[LEGACYBRANCH_FALSE]   = LEGACYBRANCH_FALSE_VERSION;
    m1[LEGACYBRANCH_71XX]    = LEGACYBRANCH_71XX_VERSION;
    m1[LEGACYBRANCH_96XX]    = LEGACYBRANCH_96XX_VERSION;
    m1[LEGACYBRANCH_173XX]   = LEGACYBRANCH_173XX_VERSION;
    m1[LEGACYBRANCH_304XX]   = LEGACYBRANCH_304XX_VERSION;
    m1[LEGACYBRANCH_340XX]   = LEGACYBRANCH_340XX_VERSION;
    m1[LEGACYBRANCH_367XX]   = LEGACYBRANCH_367XX_VERSION;
    m1[LEGACYBRANCH_390XX]   = LEGACYBRANCH_390XX_VERSION;
    m1[LEGACYBRANCH_470XX]   = LEGACYBRANCH_470XX_VERSION;
    m1[LEGACYBRANCH_UNKNOWN] = LEGACYBRANCH_UNKNOWN_VERSION;

    m2[LEGACYBRANCH_FALSE_VERSION]   = LEGACYBRANCH_FALSE;
    m2[LEGACYBRANCH_71XX_VERSION]    = LEGACYBRANCH_71XX;
    m2[LEGACYBRANCH_96XX_VERSION]    = LEGACYBRANCH_96XX;
    m2[LEGACYBRANCH_173XX_VERSION]   = LEGACYBRANCH_173XX;
    m2[LEGACYBRANCH_304XX_VERSION]   = LEGACYBRANCH_304XX;
    m2[LEGACYBRANCH_340XX_VERSION]   = LEGACYBRANCH_340XX;
    m2[LEGACYBRANCH_367XX_VERSION]   = LEGACYBRANCH_367XX;
    m2[LEGACYBRANCH_390XX_VERSION]   = LEGACYBRANCH_390XX;
    m2[LEGACYBRANCH_470XX_VERSION]   = LEGACYBRANCH_470XX;
    m2[LEGACYBRANCH_UNKNOWN_VERSION] = LEGACYBRANCH_UNKNOWN;

    return true;
}
static map<legacybranch_t, string> legacybranch_enum2ver_map;
static map<string, legacybranch_t> legacybranch_ver2enum_map;
static bool _dummy1 = create_legacybranch_ver2enum_map(legacybranch_enum2ver_map, legacybranch_ver2enum_map);

bool create_legacybranch_enum2array_map(map<legacybranch_t, string> &m)
{
    m[LEGACYBRANCH_FALSE]   = LEGACYBRANCH_FALSE_ARRAY;
    m[LEGACYBRANCH_71XX]    = LEGACYBRANCH_71XX_ARRAY;
    m[LEGACYBRANCH_96XX]    = LEGACYBRANCH_96XX_ARRAY;
    m[LEGACYBRANCH_173XX]   = LEGACYBRANCH_173XX_ARRAY;
    m[LEGACYBRANCH_304XX]   = LEGACYBRANCH_304XX_ARRAY;
    m[LEGACYBRANCH_340XX]   = LEGACYBRANCH_340XX_ARRAY;
    m[LEGACYBRANCH_367XX]   = LEGACYBRANCH_367XX_ARRAY;
    m[LEGACYBRANCH_390XX]   = LEGACYBRANCH_390XX_ARRAY;
    m[LEGACYBRANCH_470XX]   = LEGACYBRANCH_470XX_ARRAY;
    m[LEGACYBRANCH_UNKNOWN] = LEGACYBRANCH_UNKNOWN_ARRAY;
    return true;
}
static map<legacybranch_t, string> legacybranch_enum2array_map;
static bool _dummy2 = create_legacybranch_enum2array_map(legacybranch_enum2array_map);


// current branch arrays and mappings
const string CURRENT_CLOSED_ARRAY = "nv_current_pci_ids[]";
const string CURRENT_OPEN_ARRAY = "nv_current_open_pci_ids[]";

bool create_currentbranch_enum2array_map(map<kernelopen_t, string> &m)
{
    m[KERNELOPEN_FALSE] = CURRENT_CLOSED_ARRAY;
    m[KERNELOPEN_TRUE]  = CURRENT_OPEN_ARRAY;
    return true;
}
static map<kernelopen_t, string> currentbranch_enum2array_map;
static bool _dummy3 = create_currentbranch_enum2array_map(currentbranch_enum2array_map);


// function prototypes
void print_usage(char* progname);
legacybranch_t set_legacy_branch(string legacybranch);
string get_legacy_branch(legacybranch_t legacybranch);
void parse_json(Json::Value const &root);
void inject_nvidia_device(string devid, string subdevid, string name, legacybranch_t legacybranch,
                          kernelopen_t kernelopen);
void print_text();
void print_nvidia_detect();
stringstream print_nvidia_devices(legacybranch_t legacybranch, kernelopen_t kernelopen);


/*
 * Main program
 */
int main(int argc, char** argv)
{
    // process the command-line arguments
    char* prog_name = argv[0];
    int print_txt = 0;
    int print_nv_detect = 1;
    string jsonFileName = "supported-gpus.json";

    const char* const optstring = "nth";
    const option longopts[] =
    {
        {"nvidia-detect", no_argument, nullptr, 'n'},
        {"text", no_argument, nullptr, 't'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };
    int longindex = 0;
    string optname;

    while (true)
    {
        int c = getopt_long(argc, argv, optstring, longopts, &longindex);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 't':
                print_txt = 1;
                print_nv_detect = 0;
                break;

            case 'n':
                print_txt = 0;
                print_nv_detect = 1;
                break;

            case 0:
                optname = longopts[longindex].name;
                cout << "Unhandled long_option: " << optname << endl;

            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                print_usage(prog_name);
                return EXIT_FAILURE;
        }
    }

    // non-option arguments
    if (optind < argc)
    {
        jsonFileName = argv[optind];
    }

    // open the JSON file and read it into the string
    ifstream jsonFile{ jsonFileName };

    if (!jsonFile)
    {
        cerr << "Error opening JSON file: " << jsonFileName << endl;
        return EXIT_FAILURE;
    }

    string jsonFileContents{ istreambuf_iterator<char>(jsonFile), istreambuf_iterator<char>() };

    // parse the raw JSON string
    constexpr bool shouldUseOldWay = true;
    JSONCPP_STRING err;
    Json::Value root;

    if (shouldUseOldWay)
    {
        Json::Reader reader;
        reader.parse(jsonFileContents, root);
    }
    else
    {
        Json::CharReaderBuilder builder;
        const unique_ptr<Json::CharReader> reader(builder.newCharReader());

        if (!reader->parse(jsonFileContents.c_str(), jsonFileContents.c_str() + jsonFileContents.length(), &root,
                           &err))
        {
            cerr << "error: " << err << endl;
            return EXIT_FAILURE;
        }
    }

    // parse JSON data into our deviceinfo data format
    parse_json(root);

    // inject legacybranch devices missing from the 5xx.xx JSON file
    inject_nvidia_device("0x0FC0", "", "GeForce GT 640", LEGACYBRANCH_470XX, KERNELOPEN_FALSE);
    inject_nvidia_device("0x0FC1", "", "GeForce GT 640", LEGACYBRANCH_470XX, KERNELOPEN_FALSE);
    inject_nvidia_device("0x0FC2", "", "GeForce GT 630", LEGACYBRANCH_470XX, KERNELOPEN_FALSE);
    inject_nvidia_device("0x0FF3", "", "GeForce GT 745A", LEGACYBRANCH_470XX, KERNELOPEN_FALSE);
    inject_nvidia_device("0x137D", "", "GeForce GT 940A", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);

    // inject current devices missing from the 5xx.xx JSON file
    inject_nvidia_device("0x1F09", "", "NVIDIA Graphics Device", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x1BB3", "", "Tesla P4", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x1DF5", "", "Tesla V100-SXM2-32GB", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x1EB4", "", "NVIDIA T4G", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x1EB8", "", "Tesla T4", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x20B1", "", "NVIDIA A100-PCIE-40GB", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x20F0", "", "NVIDIA A100-PG506-207", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);
    inject_nvidia_device("0x20F2", "", "NVIDIA A100-PG506-217", LEGACYBRANCH_FALSE, KERNELOPEN_FALSE);

    // print deviceinfo data
    if (print_txt)
    {
        print_text();
    }
    else if (print_nv_detect)
    {
        print_nvidia_detect();
    }

    return EXIT_SUCCESS;
}


// functions
void print_usage(char* progname)
{
    cerr << "Usage: " << progname << " [supported-gpus.json]" << endl
         << "-n,--nvidia-detect  :  output the nvidia-detect.h header file (default)" << endl
         << "-t,--text           :  output text dump of device info" << endl
         << "-h,--help           :  show help" << endl
         << endl;
}


legacybranch_t set_legacy_branch(string legacybranch)
{
    if (legacybranch_ver2enum_map.find(legacybranch) != legacybranch_ver2enum_map.end())
    {
        return legacybranch_ver2enum_map[legacybranch];
    }
    else
    {
        return LEGACYBRANCH_UNKNOWN;
    }
}


string get_legacy_branch(legacybranch_t legacybranch)
{
    string output;

    for (auto iter  = legacybranch_ver2enum_map.begin();
         iter != legacybranch_ver2enum_map.end();
         iter++)
    {
        if (iter->second == legacybranch)
        {
            output = iter->first;
        }
    }

    return output;
}


void parse_json(Json::Value const &root)
{
    device_info_t device_info;

    Json::Value chips = root["chips"];

    for (Json::Value::ArrayIndex i = 0; i != chips.size(); i++)
    {
        if (chips[i].isMember("devid"))
        {
            device_info.devid = chips[i]["devid"].asString();
        }

        if (chips[i].isMember("subdevid"))
        {
            device_info.subdevid = chips[i]["subdevid"].asString();
        }

        if (chips[i].isMember("name"))
        {
            device_info.name = chips[i]["name"].asString();
        }

        if (chips[i].isMember("legacybranch"))
        {
            string legacybranch = chips[i]["legacybranch"].asString();
            device_info.legacybranch = set_legacy_branch(legacybranch);

            if (device_info.legacybranch == LEGACYBRANCH_UNKNOWN)
            {
                cerr << "FIXME: Unknown legacybranch = " << legacybranch << endl;
            }
        }
        else
        {
            device_info.legacybranch = LEGACYBRANCH_FALSE;
        }

        if (chips[i].isMember("features"))
        {
            Json::Value features = chips[i]["features"];
            device_info.kernelopen = KERNELOPEN_FALSE;

            for (Json::Value::ArrayIndex f = 0; f != features.size(); f++)
            {
                if (features[f].asString() == "kernelopen")
                {
                    device_info.kernelopen = KERNELOPEN_TRUE;
                }
            }
        }

        // add the device info to the devices map
        devices_map_iter = devices_map.find(device_info.devid);

        if (devices_map_iter == devices_map.end())
        {
            devices_map[device_info.devid] = device_info;
        }
        else
        {
            // override the entry if the existing one has non-empty subdevid
            // and the new one has an empty subdevid
            if (!devices_map_iter->second.subdevid.empty() && device_info.subdevid.empty())
            {
                cout << "Replacing devid/subdevid = ("
                     << devices_map_iter->second.devid << ","
                     << devices_map_iter->second.subdevid << ") "
                     << "with devid = ("
                     << device_info.devid
                     << ")" << endl;

                devices_map_iter->second = device_info;
            }
        }
    }
}


// injects a missing device that is not in the JSON file
void inject_nvidia_device(string devid, string subdevid, string name, legacybranch_t legacybranch,
                          kernelopen_t kernelopen)
{
    device_info_t new_device_info = {devid, subdevid, name, legacybranch, kernelopen};
    devices_map[devid] = new_device_info;
}


// debug prints every parsed device
void print_text()
{
    for (devices_map_iter  = devices_map.begin();
         devices_map_iter != devices_map.end();
         devices_map_iter++)
    {
        cout << "devid=" << devices_map_iter->second.devid << endl
             << "name=" << devices_map_iter->second.name << endl
             << "legacybranch=" << get_legacy_branch(devices_map_iter->second.legacybranch) << endl
             << "kernelopen=" << (devices_map_iter->second.kernelopen ? "true" : "false") << endl
             << endl;
    }
}


// prints nvidia-detect header file
void print_nvidia_detect()
{
    stringstream nvidia_header;
    nvidia_header << "/*" << endl
                  << " *  nvidia-detect.h - PCI device_ids for NVIDIA graphics cards" << endl
                  << " *" << endl
                  << " *  Copyright (C) 2013-2025 Philip J Perry <phil@elrepo.org>" << endl
                  << " *" << endl
                  << " *  This program is free software; you can redistribute it and/or modify" << endl
                  << " *  it under the terms of the GNU General Public License as published by" << endl
                  << " *  the Free Software Foundation; either version 2 of the License, or" << endl
                  << " *  (at your option) any later version." << endl
                  << " *" << endl
                  << " *  This program is distributed in the hope that it will be useful," << endl
                  << " *  but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl
                  << " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl
                  << " *  GNU General Public License for more details." << endl
                  << " *" << endl
                  << " *  You should have received a copy of the GNU General Public License" << endl
                  << " *  along with this program; if not, write to the Free Software" << endl
                  << " *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA." << endl
                  << " */" << endl
                  << endl
                  << "#ifndef _NVIDIA_DETECT_H" << endl
                  << "#define _NVIDIA_DETECT_H" << endl
                  << endl
                  << "typedef unsigned short u_int16_t;" << endl;

    stringstream nvidia_footer;
    nvidia_footer << "#endif  /* _NVIDIA_DETECT_H */" << endl;

    cout << nvidia_header.str() << endl
         << print_nvidia_devices(LEGACYBRANCH_71XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_96XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_173XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_304XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_340XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_367XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_390XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_470XX, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_FALSE, KERNELOPEN_FALSE).str() << endl
         << print_nvidia_devices(LEGACYBRANCH_FALSE, KERNELOPEN_TRUE).str() << endl
         << nvidia_footer.str() << endl;
}


// prints a single group of devices for the nvidia-detect header file
stringstream print_nvidia_devices(legacybranch_t legacybranch, kernelopen_t kernelopen)
{
    const int device_row_limit = 10;
    stringstream nvidia_output;

    // print current branch devices
    if (legacybranch == LEGACYBRANCH_FALSE)
    {
        if (currentbranch_enum2array_map.find(kernelopen) != currentbranch_enum2array_map.end())
        {
            nvidia_output << "/* PCI device_ids supported by the current "
                          << (kernelopen ? "open " : "proprietary ")
                          << "driver */" << endl
                          << "static const u_int16_t " << currentbranch_enum2array_map[kernelopen] << " = {";

            int x = 0;

            for (devices_map_iter  = devices_map.begin();
                 devices_map_iter != devices_map.end();
                 devices_map_iter++)
            {
                if (kernelopen == devices_map_iter->second.kernelopen)
                {
                    // insert tab every 10 devices
                    if (x % device_row_limit == 0)
                    {
                        nvidia_output << endl << ONE_TAB;
                    }

                    if (x % device_row_limit >= 1)
                    {
                        nvidia_output << ONE_SPACE;
                    }

                    nvidia_output << devices_map_iter->first << ",";

                    x++;
                }
            }

            nvidia_output << endl << "};" << endl;
        }
    }
    // print legacy branch devices
    else
    {
        if (legacybranch_enum2array_map.find(legacybranch) != legacybranch_enum2array_map.end())
        {
            nvidia_output << "/* PCI device_ids supported by the "
                          << legacybranch_enum2ver_map[legacybranch]
                          << " legacy driver */" << endl
                          << "static const u_int16_t " << legacybranch_enum2array_map[legacybranch] << " = {";

            int x = 0;

            for (devices_map_iter  = devices_map.begin();
                 devices_map_iter != devices_map.end();
                 devices_map_iter++)
            {
                if (legacybranch == devices_map_iter->second.legacybranch)
                {
                    // insert tab every 10 devices
                    if (x % device_row_limit == 0)
                    {
                        nvidia_output << endl << ONE_TAB;
                    }

                    if (x % device_row_limit >= 1)
                    {
                        nvidia_output << ONE_SPACE;
                    }

                    nvidia_output << devices_map_iter->first << ",";

                    x++;
                }
            }

            nvidia_output << endl << "};" << endl;
        }
    }

    return nvidia_output;
}


