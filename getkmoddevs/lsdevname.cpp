/*
 * Description:
 * This app looks up the given PCI/USB VendorID and DeviceID and prints the corresponding name.
 *
 * Author: Tuan Hoang <tqhoang@elrepo.org>
 */
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include <getopt.h>
extern char *optarg;
extern int optind, opterr, optopt;


// line type prefixes
const string ONE_TAB("\t");
const string TWO_TABS("\t\t");

// line delimiter
const string ONE_SPACE(" ");
const string TWO_SPACES("  ");

// typedefs
typedef map<string, string> vendors_map_t;
typedef map<string, map<string, string>> devices_map_t;

// function prototypess
void print_usage(char* progname);
string str_tolower(string s);
void parse_ids(string const& ids_file, vendors_map_t& vendors_map, devices_map_t& devices_map);
void print_all_ids(vendors_map_t& vendors_map, devices_map_t& devices_map);
void print_id(vendors_map_t& vendors_map, devices_map_t& devices_map,
		string const& vendor_id, string const& device_id,
		bool print_numbers, bool print_all);


/*
 * Function to print usage
 */
void print_usage(char* progname)
{
	cerr << "Usage: " << progname << endl
	     << "--pcifile <pci.ids>  :  pci.ids file path" << endl
	     << "--usbfile <usb.ids>  :  usb.ids file path" << endl
	     << "-v,--vendor <xxxx>   :  vendor ID" << endl
	     << "-d,--device <xxxx>   :  device ID" << endl
	     << "-p,--pci             :  pci type device (default)" << endl
	     << "-u,--usb             :  usb type device" << endl
	     << "-a,--all             :  prints all devices" << endl
	     << "-n,--numbers         :  prints device numbers" << endl
	     << "-h,--help            :  show help" << endl
	     << endl;
}


/*
 * main program
 */
int main(int argc, char** argv)
{
	char* prog_name = argv[0];

	if (argc == 1)
	{
		print_usage(prog_name);
		return EXIT_FAILURE;
	}

	map<string, string> vendors_map;
	map<string, map<string, string>> devices_map;

	int print_all = 0;
	int print_numbers = 0;
	int type_pci = 0;
	int type_usb = 0;
	string vendor_id, device_id;
	string pci_ids_file = "/usr/share/hwdata/pci.ids";
	string usb_ids_file = "/usr/share/hwdata/usb.ids";


	const char* const optstring = "v:d:puanh";
	const option long_options[] = {
		{"pcifile", required_argument, nullptr, 0},
		{"usbfile", required_argument, nullptr, 0},
		{"vendor", required_argument, nullptr, 'v'},
		{"device", required_argument, nullptr, 'd'},
		{"pci", no_argument, nullptr, 'p'},
		{"usb", no_argument, nullptr, 'u'},
		{"all", no_argument, nullptr, 'a'},
		{"numbers", no_argument, nullptr, 'n'},
		{"help", no_argument, nullptr, 'h'},
		{nullptr, no_argument, nullptr, 0}
	};
	int option_index;
	string optname;

	while (true)
	{
		const auto opt = getopt_long(argc, argv, optstring, long_options, &option_index);

		if (-1 == opt)
			break;

		switch (opt)
		{
		       case 0:
				optname = long_options[option_index].name;
            			if (optname == "pcifile")
				{
					pci_ids_file = optarg;
					//cout << "pci_ids_file = " << pci_ids_file << endl;
				}
				else if (optname == "usbfile")
				{
					usb_ids_file = optarg;
					//cout << "usb_ids_file = " << usb_ids_file << endl;
				}
				break;

			case 'v':
				vendor_id = optarg;
				break;

			case 'd':
				device_id = optarg;
				break;

			case 'p':
				type_pci = 1;
				break;

			case 'u':
				type_usb = 1;
				break;

			case 'a':
				print_all = 1;
				break;

			case 'n':
				print_numbers = 1;
				break;

			case 'h': // -h or --help
			case '?': // Unrecognized option
			default:
				print_usage(prog_name);
				return EXIT_FAILURE;
		}
	}

	// set default type
	if ((!type_pci && !type_usb) || (type_pci && type_usb))
	{
		type_pci = 1;
		type_usb = 0;
	}
	
	// parse the hwdata ids files
	if (type_pci)
	{
		parse_ids(pci_ids_file, vendors_map, devices_map);
	}
	else
	{
		parse_ids(usb_ids_file, vendors_map, devices_map);
	}

	// convert vendor_id and device_id to lowercase
	vendor_id = str_tolower(vendor_id);
	device_id = str_tolower(device_id);

	// print the names
	if (!vendor_id.empty())
	{
		print_id(vendors_map, devices_map, vendor_id, device_id, print_numbers, print_all);
	}
	else
	{
		print_all_ids(vendors_map, devices_map);
	}

	return EXIT_SUCCESS;
}


/*
 * Function to convert a string to lowercase
 */
string str_tolower(string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c){ return std::tolower(c); });
	return s;
}


/*
 * Function to parse the hwdata ids files
 */
void parse_ids(string const& ids_file, vendors_map_t& vendors_map, devices_map_t& devices_map)
{
	// file handling and parsing
	ifstream fin(ids_file);
	string line;
	string vendor_id, device_id;
	string vendor_name, device_name;

	// read through the file
	while (getline(fin, line))
	{
		// skip comments or only whitespace lines
		if (line[0] == '#')
		{
			continue;
		}
		else
		{
			// check for whitespace only
			string tmp = line;
			tmp.erase(std::remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
			if (tmp.empty())
			{
				continue;
			}
		}

		// parse the data lines
		if (line.find(TWO_TABS) != string::npos)
		{
			// ignore sub-vendor and sub-device info
			continue;
		}
		else if (line.find(ONE_TAB) != string::npos)
		{
			// device info
			line = line.substr(line.find(ONE_TAB) + ONE_TAB.length());

		        if (line.find(TWO_SPACES) != string::npos)
        		{
                		device_id   = line.substr(0, line.find(TWO_SPACES));
                		device_name = line.substr(line.find(TWO_SPACES) + TWO_SPACES.length());

				devices_map_t::iterator devices_iter = devices_map.find(vendor_id);
				if (devices_iter != devices_map.end())
				{
					devices_iter->second[device_id] = device_name;
				}
        		}
		}
		else
		{
			// vendor info
		        if (line.find(TWO_SPACES) != string::npos)
        		{
                		vendor_id   = line.substr(0, line.find(TWO_SPACES));
                		vendor_name = line.substr(line.find(TWO_SPACES) + TWO_SPACES.length());

				vendors_map[vendor_id] = vendor_name;
				devices_map[vendor_id] = map<string, string>();
        		}
		}
	}
}


/*
 * Function to print all hwdata ids
 */
void print_all_ids(vendors_map_t& vendors_map, devices_map_t& devices_map)
{
	for (auto ven_iter : devices_map)
	{
		cout << ven_iter.first << TWO_SPACES << vendors_map[ven_iter.first] << endl;

		for (auto dev_iter : ven_iter.second)
		{
			cout << ONE_TAB << dev_iter.first << TWO_SPACES << dev_iter.second << endl;
		}

		cout << endl;
	}
}


/*
 * Function to print one hwdata id
 */
void print_id(vendors_map_t& vendors_map, devices_map_t& devices_map,
		string const& vendor_id, string const& device_id,
		bool print_numbers, bool print_all)
{
	string vendor_name, device_name;

	if (vendors_map.find(vendor_id) != vendors_map.end())
	{
		vendor_name = vendors_map[vendor_id];
	}
	else
	{
		vendor_name = "UNKNOWN VENDOR " + vendor_id; 
	}

	if (!device_id.empty())
	{
		if (devices_map[vendor_id].find(device_id) != devices_map[vendor_id].end())
		{
			device_name = devices_map[vendor_id][device_id];
		}
		else
		{
			device_name = "UNKNOWN DEVICE " + device_id;
		}

		if (print_numbers)
			cout << "[" << vendor_id << ":" << device_id << "]" << ONE_SPACE;

		cout << vendor_name << ONE_SPACE << device_name;
	}
	else if (print_all)
	{
		auto ven_iter = devices_map.find(vendor_id);
		if (ven_iter != devices_map.end())
		{
			for (auto dev_iter : ven_iter->second)
			{
				if (print_numbers)
					cout << "[" << vendor_id << ":" << dev_iter.first << "]" << ONE_SPACE;

				cout << vendors_map[ven_iter->first] << ONE_SPACE << dev_iter.second << endl;
			}
		}
	}
	else
	{
		if (print_numbers)
			cout << "[" << vendor_id << ":****]" << ONE_SPACE;

		cout << vendor_name;
	}

	cout.flush();
}
