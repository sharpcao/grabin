/*
	Grabin is to view the hex data of file and rewrite to another file.  
	
	author:Sharp.Cao
	Date:2025/1/25

*/


#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <stdexcept>
using namespace std;



class Param_t
{
public:
	string input_file = "";
	unsigned long start_offset = 0;
	unsigned long dump_size = 0;
	bool b_overwrite_part = false;
	string output_file = "";
	unsigned long outfile_address =0;


	void parse_args(int, const char**);
	void print_args(){
		cout << "input_file: " << input_file << endl
			<< "start_offset: " << start_offset << endl
			<< "dump_size: " << dump_size << endl
			<<"output_file: " << output_file << endl;
	}
private:
	long getFileSize(const std::string& filename) ;
	
};

long Param_t::getFileSize(const std::string& filename) 
{
    std::ifstream file(filename, std::ifstream::binary);
    if (file) {
        file.seekg(0, file.end);
        long length = file.tellg();
        file.close();
        return length;
    }
    return 0; // if open file failed, return size 0
}

void exit_with_message(int, const string& );

unsigned long conv_num(const string& s)
{
	unsigned long result = 0;
	try {
		if (s.size() >1 && s.back() == 'h'){
			result =  stol(s.substr(0,s.size()-1),0,16);
		}else{

			result =  stol(s);
		}
	}catch(...){
		exit_with_message(999,"convert to digit number wrong!");
	}
	return result;
}

void Param_t::parse_args(int argc, const char* argv[])
{
	if (argc < 2){
		exit_with_message(1,"too few arguments");
	}

	input_file = argv[1];
		
	for(int i=2; i < argc; ++i)
	{
		if (i ==2 && argv[2][0] != '-'){
			dump_size = conv_num(string(argv[2]));
			continue;
		}
		string sw = argv[i];
		if (sw =="-o" || sw =="-ow" || sw=="-oa"){
			if (++i != argc) {
				output_file = argv[i];

				if (sw != "-o") {b_overwrite_part = true;}
				if (sw =="-oa"){
					outfile_address = getFileSize(output_file);
				}


			}else{
				exit_with_message(3,"error output file");
			}
			

		}else if (sw=="-s"){
			if (++i != argc) {
				start_offset = conv_num(string(argv[i]));
			}else{
				exit_with_message(3,"error start offset");
			}	
		}else if (sw =="-wa"){
			if (++i != argc) {
				outfile_address = conv_num(string(argv[i]));
			}else{
				exit_with_message(3,"error write file start offset");
			}

		}else if(sw=="-e"){
			if (++i != argc) {
				auto end_offset = conv_num(string(argv[i]));
				dump_size = end_offset - start_offset + 1;
			}else{
				exit_with_message(3,"error end offset");
			}	
		}
		else{
			exit_with_message(4,"error command");
		}
	}

	if (dump_size == 0 ){
		if(input_file[0] =='\\') {
			dump_size = 1;	
		}
		else{
			ifstream ftmp(input_file,ios::binary| ios::ate);
			if (ftmp.is_open()){
				dump_size = static_cast<unsigned long>(ftmp.tellg()) - start_offset;
			}else{
				exit_with_message(5,"can't open input file");
			}
		}
	}
	
}


void exit_with_message(int exit_code, const string& msg)
{
	cout << "error code:" << exit_code << ": " << msg << endl;
	cout << "usage: grabin <filename> <size> [options]" << endl
			<<"options:" << endl
			<<"\t" << "-s <start offset>" << endl
			<<"\t" << "-e <end offset>" << endl
			<<"\t" << "-o <output filename>" << endl
			<<"\t" << "-oa <output filename>" << endl
			<<"\t" << "-ow <output filename> -wa <address>" << endl
			<<"*note: add h suffix for hex digit, eg. 0Ah" << endl;
	exit(exit_code);
}


ostream& operator<<(ostream& os, const vector<char>& buf)
{
	for (char b:buf){
		os << " " << hex << setw(2) << setfill('0') <<
				static_cast<int>((unsigned char)b);
	}
	return os;
}
ofstream& operator<<(ofstream& of,const vector<char>& buf)
{
	of.write(buf.data(),buf.size());
	return of;
}


int main(int argc, const char* argv[])
{
	Param_t param;
	param.parse_args(argc, argv);

	//param.print_args();

	auto size = param.dump_size;
	vector<char> buf(size);
	if (param.input_file[0]=='\\'){
		auto value = conv_num(param.input_file.substr(1,param.input_file.size()-1));
		if (value >255){ exit_with_message(999,"input value too large");}
		for(auto &it:buf){
			it = value;
		}

	}else {
		ifstream file(param.input_file, ios::binary | ios::in);
		if (!file.is_open()){
			exit_with_message(5,"can't open input file");
		}
		file.seekg(param.start_offset);
		file.read(buf.data(), size);
		file.close();
	}


	if (param.output_file !=""){
		ofstream outfile;
		if (param.b_overwrite_part){
			outfile.open(param.output_file, ios::binary | ios::out | ios::in);
		}else{
			outfile.open(param.output_file, ios::binary | ios::out);
		}
		if(outfile.is_open()){
			outfile.seekp(param.outfile_address,ios::beg);
			outfile.write(buf.data(),buf.size());
			outfile.close();
		}else{
			exit_with_message(6,"can't open output file");
		}


	}else{
		cout << buf << endl;
	}

	
	return 0;
} 

