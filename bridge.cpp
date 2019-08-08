#include "bridge.h"

void bridge::print()
{
	cout<<id<<" ";
	for(int i=0;i<connected_LAN.size();i++)
	{
		cout<<connected_LAN[i]<<" ";
	}
	cout<<endl;
}

int bridge::get_id()
{
	return id;
}

vector<char> bridge::get_connected_LAN()
{
	return connected_LAN;
}

map<char,int> bridge::get_port_map()
{
	return port_map;
}

bridge::bridge()
{
	time = 0;
	is_generating = 1;
    bridge::config_message cm(id,0,id);
    best_config = cm;
}

bridge::bridge(int i,vector<char> v)
{
	time = 0;
	is_generating = 1;
	id = i;
	connected_LAN = v;
	for(int i=0;i<v.size();i++)
	{
		port_map[v[i]] = 1;
	}
	bridge::config_message cm(id,0,id);
	best_config = cm;
}

bool bridge::operator < (bridge const &b)
{
	return (id < b.id);
}

bool bridge::is_configured()
{
	return (root == 1);
}

void bridge::set_time(int time)
{
	this->time = time;
}

char bridge::get_root_port()
{
	return root_port;
}

int bridge::get_dist()
{
	return distance_from_root;
}

void bridge::add_msg(config_message m,char c)
{
	pair<config_message,char> p(m,c);
	msg_table.push_back(p);
}

void bridge::send_message(bridge *b,char c)
{
	if(port_map[c] != 0)
	{
		int r = best_config.root;
		int d = best_config.distance_from_root;
		int i = id;
		bridge::config_message cm1(r,d+1,i);
		bridge::config_message cm2(r,d,i);
		if(is_generating) b->add_msg(cm2,c);
		else b->add_msg(cm1,c);
	}
}

bool bridge::compare_config(bridge::config_message m1, bridge::config_message m2)
{
	int r1 = m1.root,r2 = m2.root;
	int d1 = m1.distance_from_root,d2 = m2.distance_from_root;
	int i1 = m1.id,i2 = m2.id; 
	bool b = false;
	if(r1 < r2) b = true;
	else if(r1 == r2 && d1 < d2) b = true;
	else if(r1 == r2 && d1 == d2 && i1 < i2) b = true;
	return b;
}

void bridge::process_messages()
{
	bridge::config_message curr_best = best_config;
	char new_root_port = root_port;

	for(int i=0;i<msg_table.size();i++)
	{
		if(compare_config(msg_table[i].first,curr_best)) 
		{
			curr_best = msg_table[i].first;
			new_root_port = msg_table[i].second;
		}
	}

	best_config = curr_best;

	if(best_config.root == id)
	distance_from_root = best_config.distance_from_root;
	else distance_from_root = best_config.distance_from_root + 1;

	root = best_config.root;

	bridge::config_message cm(root,distance_from_root,id);

	for(int i=0;i<msg_table.size();i++)
	{
		if(compare_config(msg_table[i].first,cm))
		{
			if(new_root_port != msg_table[i].second)
			port_map[msg_table[i].second] = 0;
		}
	}

	if(root_port != new_root_port)
	{
		map<char,int>::iterator itr;
		for(itr=port_map.begin();itr!=port_map.end();itr++)
		{
			port_map[itr->first] = 1;
		}
	}

	root_port = new_root_port;
	vector<pair<config_message,char> > c;
	msg_table = c;
	if(best_config.root != id) is_generating = false;
	time ++;
}

void bridge::print_best_config()
{
	cout<<root<<" "<<distance_from_root<<" "<<id<<endl;
}

void bridge::print_msg_table()
{
	if( !(msg_table.size() == 0) )
	{
		for(int i=0;i<msg_table.size();i++)
		{
			cout<<msg_table[i].first.root<<" "<<msg_table[i].first.distance_from_root<<" "<<msg_table[i].first.id<<endl;
		}
	}
}

void bridge::print_received()
{
	if( !(msg_table.size() == 0) )
	{
	for(int i=0;i<msg_table.size();i++)
	{
		cout<<time+1<<" "<<"B"<<id<<" "<<"r"<<" "<<"("<<"B"<<msg_table[i].first.root<<", "<<msg_table[i].first.distance_from_root<<", "<<"B"<<msg_table[i].first.id<<")"<<endl;
	}
    }
}

void bridge::print_sent()
{
	int r = best_config.root;
	int d = best_config.distance_from_root;
	int i = id;
	bridge::config_message cm1(r,d+1,i);
	bridge::config_message cm2(r,d,i);
	bridge::config_message sent_msg(r,d,i);
	if(is_generating) sent_msg = cm2;
	else sent_msg = cm1;
	cout<<time<<" "<<"B"<<id<<" "<<"s"<<" "<<"("<<"B"<<sent_msg.root<<", "<<sent_msg.distance_from_root<<", "<<"B"<<sent_msg.id<<")"<<endl;
}

map<int,char> bridge::get_forwarding_table()
{
	return forwarding_table;
}

vector<bridge::message> bridge::get_message_table()
{
	return message_table;
}

void bridge::set_forwarding_table(map<int,char> forwarding_table)
{
	this->forwarding_table = forwarding_table;
}

void bridge::set_message_table(vector<bridge::message> message_table)
{
	this->message_table = message_table;
}

void bridge::add_message(char sender_lan,char receiver_lan,int sender_host,int receiver_host)
{
	bridge::message new_message(sender_lan,receiver_lan,sender_host,receiver_host);
	message_table.push_back(new_message);
}

void bridge::forward_on_port(char port, bridge::message m, map<char,vector<bridge> > lan_table, vector<bridge>* ext_lan)
{
	vector<bridge> v = lan_table[port];
	for(int i=0;i<v.size();i++)
	{
		if(v[i].get_id() != id) 
		{
			(*ext_lan)[v[i].get_id()-1].add_message(port,m.receiver_lan,m.sender_host,m.receiver_host);
		}
	}
}

