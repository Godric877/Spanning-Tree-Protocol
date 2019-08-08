#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "bridge.h"

using namespace std;

vector<bridge> ext_lan;
map<char,vector<bridge> > lan_table;
map<int,char> host_table;
vector<pair<int,int> > transfers;
map<char,vector<bridge> > final_lan_table;
int t = 0;

int string_to_int(string s)
{
	int ans = 0;
	int mult = 1;
	for(int i=s.size()-1;i>=0;i--)
	{
		char c = s[i];
		int x = c-48;
		ans+=(x*mult);
		mult*=10;
	}
	return ans;
}

void spanning_tree(bool trace_flag)
{
	if(trace_flag == 1)
	{
		for(int i=0;i<ext_lan.size();i++)
		{
			ext_lan[i].print_sent();
		}
	}
	for(int i=0;i<ext_lan.size();i++)
	{
		vector<char> v = ext_lan[i].get_connected_LAN();
		for(int j=0;j<v.size();j++)
		{
			for(int k=0;k<lan_table[v[j]].size();k++)
			{
				if(ext_lan[i].get_id() != lan_table[v[j]][k].get_id())
				{
				int b = lan_table[v[j]][k].get_id();
				ext_lan[i].send_message(& ext_lan[b-1], v[j]);
				//cout<<ext_lan[i].get_id()<<" "<<ext_lan[b-1].get_id()<<endl;
				}
			}
		} 
	}
	// for(int i=0;i<ext_lan.size();i++)
	// {
	// 	cout<<"Bridge "<<i+1<<" "<<endl;
	// 	ext_lan[i].print_msg_table();
	// 	cout<<endl;
	// }
	if(trace_flag == 1)
	{
		for(int i=0;i<ext_lan.size();i++)
		{
			ext_lan[i].print_received();
		}
	}
	for(int i=0;i<ext_lan.size();i++)
	{
		ext_lan[i].process_messages();
	}
	t++;
	// for(int i=0;i<ext_lan.size();i++)
	// {
	// 	cout<<i+1<<" ";ext_lan[i].print_best_config();
	// }
}

void print_forwarding_tables()
{
	for(int i=0;i<ext_lan.size();i++)
	{
		cout<<"B"<<ext_lan[i].get_id()<<":"<<endl;
		cout<<"HOST ID | FORWARDING PORT"<<endl;
		map<int,char> forwarding_table = ext_lan[i].get_forwarding_table();
		map<int,char>::iterator itr;
		for(itr=forwarding_table.begin();itr!=forwarding_table.end();itr++)
		{
			cout<<"H"<<itr->first<<" | "<<itr->second<<endl;
		}
	}
}

void routing_helper(char sender_lan,char receiver_lan,int sender_host,int receiver_host,bool trace_flag)
{
	bool b = 0;

	while(!b)
	{
		b = 1;
		map<int,int> waiting_to_send;
		for(int i=0;i<ext_lan.size();i++)
		{
			vector<bridge::message> message_table = ext_lan[i].get_message_table();
			if(message_table.size() > 0) waiting_to_send[i] = 1;
		}
		for(int i=0;i<ext_lan.size();i++)
		{
			if(waiting_to_send.find(i) != waiting_to_send.end())
			{
			map<int,char> forwarding_table = ext_lan[i].get_forwarding_table();
			vector<bridge::message> message_table = ext_lan[i].get_message_table();
			if(trace_flag)
			{
				for(int p=0;p<message_table.size();p++)
				{
					cout<<t<<" "<<"B"<<i+1<<" "<<"r"<<" "<<sender_lan<<" "<<"-->"<<" "<<receiver_lan<<endl;
				}
			}
			map<char,int> port_map = ext_lan[i].get_port_map();
			for(int j=0;j<message_table.size();j++)
			{
				if(forwarding_table.find(message_table[j].get_receiver_host_id()) != forwarding_table.end()) 
				{
					if(forwarding_table[message_table[j].get_receiver_host_id()] != message_table[j].get_sender_lan())
					{
					forwarding_table[message_table[j].get_sender_host_id()] = message_table[j].get_sender_lan();
					ext_lan[i].forward_on_port(forwarding_table[message_table[j].get_receiver_host_id()],message_table[j],final_lan_table,&ext_lan);
					if(trace_flag) cout<<t<<" "<<"B"<<i+1<<" "<<"s"<<" "<<sender_lan<<" "<<"-->"<<" "<<receiver_lan<<endl;
					} 
					else
					{
					forwarding_table[message_table[j].get_sender_host_id()] = message_table[j].get_sender_lan();
					}
				}
				else
				{
					forwarding_table[message_table[j].get_sender_host_id()] = message_table[j].get_sender_lan();
					map<char,int>::iterator itr;
					for(itr=port_map.begin();itr!=port_map.end();itr++)
					{
						if(itr->first != message_table[j].get_sender_lan() && itr->second == 1)
						{
							ext_lan[i].forward_on_port(itr->first,message_table[j],final_lan_table,&ext_lan);
							if(trace_flag) cout<<t<<" "<<"B"<<i+1<<" "<<"s"<<" "<<sender_lan<<" "<<"-->"<<" "<<receiver_lan<<endl;
						}
					}
				}
			}
			vector<bridge::message> v;
			ext_lan[i].set_forwarding_table(forwarding_table);
			ext_lan[i].set_message_table(v);
			}
		}
		t++;
		for(int i=0;i<ext_lan.size();i++)
		{
			b = b && ((ext_lan[i].get_message_table()).size() == 0);
		}
	}
}

void routing_process(bool trace_flag)
{
	for(int i=0;i<transfers.size();i++)
	{
		t = 100*(i+1);
		int sender_host = transfers[i].first;
		int receiver_host = transfers[i].second;
		char sender_lan = host_table[sender_host];
		char receiver_lan = host_table[receiver_host];
		for(int j=0;j<final_lan_table[sender_lan].size();j++)
		{
			ext_lan[ final_lan_table[sender_lan][j].get_id() - 1 ].add_message(sender_lan,receiver_lan,sender_host,receiver_host);
			//if(trace_flag) cout<<t<<" "<<"B"<<final_lan_table[sender_lan][j].get_id()<<" "<<"r"<<" "<<sender_host<<" "<<"-->"<<" "<<"-"<<endl;
		}
		t++;
		routing_helper(sender_lan,receiver_lan,sender_host,receiver_host,trace_flag);
		print_forwarding_tables();
		cout<<endl;
	}
}

bool is_configured()
{
	bool count = 1;

	for(int i=0;i<ext_lan.size();i++)
	{
		count = count && ext_lan[i].is_configured();
	}

	bool b = 1;

	map<char,vector<bridge> >::iterator itr;
	for(itr=lan_table.begin();itr!=lan_table.end();itr++)
	{
		int count = 0;
		for(int i=0;i<(itr->second).size();i++)
		{
		   int id = (itr->second)[i].get_id();
           map<char,int> port_map = ext_lan[id-1].get_port_map();
           char root_port = ext_lan[id-1].get_root_port();
           if(port_map[itr->first] == 1 && (itr->first != root_port)) count++;
		}
		b = b && (count == 1);
	}

	return (b && count);
}

void print_spanning_tree()
{
	for(int i=0;i<ext_lan.size();i++)
	{
		cout<<"B"<<ext_lan[i].get_id()<<":"<<" ";
		map<char,int> port_map = ext_lan[i].get_port_map();
		map<char,int>::iterator itr;
		int x = 1;
		int size = port_map.size();
		for(itr=port_map.begin();itr!=port_map.end();itr++)
		{
			cout<<itr->first<<"-";
			if(itr->second == 0) 
			{
				//if(itr->first == ext_lan[i].get_root_port()) cout<<"RP"<<" ";
				//else cout<<"NP"<<" ";
				cout<<"NP";
			}
			else if(itr->second == 1) 
			{
				if(itr->first == ext_lan[i].get_root_port()) cout<<"RP";
				else cout<<"DP";
			}
			if(x!=size) cout<<" ";
			x++;
		}
		cout<<endl;
	}
}

int main()
{
	bool trace_flag;
	int num_bridges;
	cin>>trace_flag>>num_bridges;
	cin.ignore();

	// Taking Input

	for(int i=1;i<num_bridges+1;i++)
	{
		string inp;
		getline(cin,inp);
		int j = 0;
		while(inp[j] != ':') {j++;}
		int bridge_id = i;
		vector<char> connected_LAN;
		for(int t=j+1;t<inp.size();t++)
		{
			if(inp[t] != ' ') 
			{
			  connected_LAN.push_back(inp[t]);
			}
		}
		bridge b(bridge_id,connected_LAN);
		ext_lan.push_back(b);
	}

	for(int i=0;i<ext_lan.size();i++)
	{
		vector<char> connected_LAN = ext_lan[i].get_connected_LAN();
		for(int j=0;j<connected_LAN.size();j++)
		{
			lan_table[connected_LAN[j]].push_back(ext_lan[i]);
		}
	}

	for(int x=0;x<lan_table.size();x++)
	{
		string inp;
		getline(cin,inp);
		int j = 0;
		while(inp[j] != ':') {j++;}
		string s = "";
		for(int t=j+1;t<inp.size();t++)
		{
			if(inp[t] == 'H')
			{
				string s = "";
				t++;
				while(inp[t] != ' ' && t < inp.size())
				{
					s = s + inp[t];
					t++;
				}
				int host_id = string_to_int(s);
				host_table[host_id] = inp[0];
			}
		}
	}

	string str_msgs;
	getline(cin,str_msgs);
	int num_msgs = string_to_int(str_msgs);

	for(int i=0;i<num_msgs;i++)
	{   
		int sender_host,receiver_host,count = 0;
		string inp;
		getline(cin,inp);
		for(int j=0;j<inp.size();j++)
		{
			string s = "";
			if(inp[j] == 'H')
			{
				count++;
				j++;
				while(inp[j] != ' ' && j < inp.size())
				{
					s = s + inp[j];
					j++;
				}
			}
			if(count == 1) sender_host = string_to_int(s);
			else if (count == 2) receiver_host = string_to_int(s);
		}
		pair<int,int> p(sender_host,receiver_host);
		transfers.push_back(p);
	}

	// Input Finished

	// Running Spanning Tree Protocol
	//int i=0;
	while(!is_configured())
	{
		//cout<<"Iteration "<<i+1<<" "<<endl;
		spanning_tree(trace_flag);
		//print_spanning_tree();
		//cout<<endl;
		//i++;
	}

	// Printing Spanning Tree
	if(is_configured()) print_spanning_tree();

	map<char,vector<bridge> >::iterator itr;
	for(itr=lan_table.begin();itr!=lan_table.end();itr++)
	{
		for(int i=0;i<(itr->second).size();i++)
		{
		   int id = (itr->second)[i].get_id();
           map<char,int> port_map = ext_lan[id-1].get_port_map();
           char root_port = ext_lan[id-1].get_root_port();
           if(port_map[itr->first] == 1)
           {
           		final_lan_table[itr->first].push_back(ext_lan[id-1]);
           		//cout<<itr->first<<" "<<id<<endl;
           }
		}
	}

	// Routing Messages
	routing_process(trace_flag);

	return 0;
}