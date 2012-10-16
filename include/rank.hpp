/* 
 * hCraft - A custom Minecraft server.
 * Copyright (C) 2012	Jacob Zhitomirsky
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _hCraft__RANK_H_
#define _hCraft__RANK_H_

#include "permissions.hpp"
#include <unordered_set>
#include <vector>


namespace hCraft {
	
	/* 
	 * 
	 */
	class group
	{
		int  power;      // groups are sorted using this field (higher power = higher-ranked group).
		char name[25];   // 24 chars max
		char col;        // name color.
		char prefix[33]; // 32 chars max
		char suffix[33]; // 32 chars max
		
		bool chat;       // if players of this group can send chat messages.
		bool build;      // whether players of this group can modify blocks.
		bool move;       // whether the players can move.
		
		permission_manager& perm_man;
		std::unordered_set<permission> perms;
		
	public:
		/* 
		 * Class constructor.
		 */
		group (permission_manager& perm_man, int power, const char *name);
		
		
		/* 
		 * Adds the specified permission node into the group's permissions list.
		 */
		void add (permission perm);
		void add (const char *perm);
		
		/* 
		 * Adds all permission nodes contained within the specified null-terminated
		 * array into the group's permissions list.
		 */
		void add (const char **perms);
		
		/* 
		 * Copies and inserts all permissions from the specified group into this
		 * one.
		 */
		void add (const group& other);
		
		
		/* 
		 * Checks whether this group has the given permission node.
		 */
		bool has (permission perm);
		bool has (const char *str);
		
		
		/* 
		 * Getter\Setters:
		 */
		
		inline int get_power () const { return this->power; }
		inline const char* get_name () { return this->name; }
		
		inline char get_color () const { return this->col; }
		inline void set_color (char col) { this->col = col ; }
		
		inline const char* get_prefix () const { return this->prefix; }
		void set_prefix (const char *val);
		
		inline const char* get_suffix () const { return this->suffix; }
		void set_suffix (const char *val);
		
		inline bool can_chat () const { return this->chat; }
		inline void can_chat (bool val) { this->chat = val; }
		inline bool can_build () const { return this->build; }
		inline void can_build (bool val) { this->build = val; }
		inline bool can_move () const { return this->move; }
		inline void can_move (bool val) { this->move = val; }
	};
	
	
	/* 
	 * A collection of groups.
	 */
	class rank
	{
		std::vector<group *> groups;
	};
}

#endif

