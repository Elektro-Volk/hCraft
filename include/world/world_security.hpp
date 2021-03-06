/* 
 * hCraft - A custom Minecraft server.
 * Copyright (C) 2012-2013	Jacob Zhitomirsky (BizarreCake)
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

#ifndef _hCraft__WORLD_SECURITY_H_
#define _hCraft__WORLD_SECURITY_H_

#include "system/security.hpp"
#include <vector>
#include <string>


namespace hCraft {
	
	class player;
	
	
	/* 
	 * Manages world ownership, members, build&join permissions, etc...
	 */
	class world_security: public ownership_security
	{
		std::string ps_build;
		std::string ps_join;
		
	public:
		const std::string& get_build_perms () const;
		const std::string& get_join_perms () const;
		
		void set_build_perms (const std::string& str);
		void set_join_perms (const std::string& str);
		
	public:
		/* 
		 * Returns true if the specified player can modify blocks in the world.
		 */
		bool can_build (player *pl) const;
		
		/* 
		 * Returns true if the specified player can join the world.
		 */
		bool can_join (player *pl) const;
	};
}

#endif

