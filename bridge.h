#include <iostream>
#include <vector>
#include <utility>
#include <map>

using namespace std;

class bridge
{
	struct config_message
	{
		int id,distance_from_root,root;
		config_message(int root,int distance_from_root,int id)
		{
			this->id = id;
			this->distance_from_root = distance_from_root;
			this->root = root;
		}
		config_message()
		{
			id = 1;
			distance_from_root = 0;
			root = 0;
		}
	};
	int id,distance_from_root,time,root;
	vector<char> connected_LAN;
	map<char,int> port_map;
	char root_port;
	bool is_generating;
	config_message best_config;
	vector<pair<config_message,char> > msg_table;
	map<int,char> forwarding_table;

	public:
	struct message
	{
		char sender_lan,receiver_lan;
		int sender_host,receiver_host;
		message(char sender_lan,char receiver_lan,int sender_host,int receiver_host)
		{
			this->sender_lan = sender_lan;
			this->receiver_lan = receiver_lan;
			this->sender_host = sender_host;
			this->receiver_host = receiver_host;
		}
		int get_sender_host_id()
		{
			return sender_host;
		}
		int get_receiver_host_id()
		{
			return receiver_host;
		}
		char get_sender_lan()
		{
			return sender_lan;
		}
	};
	vector<message> message_table;
	void print();
	bridge();
	bridge(int i,vector<char> v);
	int get_id();
	char get_root_port();
	int get_dist();
	map<char,int> get_port_map();
	vector<char> get_connected_LAN();
	void send_message(bridge *b, char c);
	void add_msg(config_message m,char c);
	void add_message(char sender_lan,char receiver_lan,int sender_host,int receiver_host);
	void forward_on_port(char port, message m, map<char,vector<bridge> > lan_table,vector<bridge>* ext_lan);
	map<int,char> get_forwarding_table();
	vector<message> get_message_table();
	void set_forwarding_table(map<int,char> forwarding_table);
	void set_message_table(vector<message> message_table);
	void print_msg_table();
	bool compare_config(config_message m1,config_message m2);
	void process_messages();
	bool operator < (bridge const &b);
	bool is_configured();
	void print_best_config();
	void set_time(int time);
	void print_received();
	void print_sent();
};