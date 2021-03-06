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

#include "commands/rank.hpp"
#include "system/server.hpp"
#include "player/player.hpp"
#include "system/sqlops.hpp"
#include "player/rank.hpp"
#include "util/stringutils.hpp"


namespace hCraft { 
	namespace commands {
		
		static bool
		is_vowel (char c)
		{
			switch (c)
				{
					case 'a':
					case 'e':
					case 'i':
					case 'o':
					case 'u':
						return true;
					
					default:
						return false;
				}
		}
		
		
		static bool
		get_next_rank (player *pl, const rank& prev, rank& out, bool up)
		{ 
			out = prev;
			group *main = out.main ();
			group *next = main->ladder
				? (up ? main->ladder->successor (main) : main->ladder->predecessor (main))
				: nullptr;
			
			if (!next)
				{
					if (main->ladder)
						{
							if (!main->ladder->has (main))
								pl->message ("§c * §7Player is in the wrong ladder§c.");
							else
								{
									if (up)
										pl->message ("§c * §7Player already at the top of the rank ladder§c.");
									else
										pl->message ("§c * §7Player already at the bottom of the rank ladder§c.");
								}
						}
					else
						pl->message ("§c * §7Player does not have an associated ladder with their rank§c.");
					return false;
				}
			
			out.replace (main, next);
			return true;
		}
		
		/* /rank
		 * 
		 * Changes the rank of a specified player.
		 * 
		 * Permissions:
		 *   - command.admin.rank
		 *       Needed to execute the command.
		 */
		void
		c_rank::execute (player *pl, command_reader& reader)
		{
			if (!pl->perm (this->get_exec_permission ()))
					return;
			
			reader.add_option ("quiet", "q");
			if (!reader.parse (this, pl))
					return;
			if (reader.arg_count () != 2)
				{ this->show_summary (pl); return; }
			
			bool quiet = reader.opt ("quiet")->found ();
			
			enum
				{
					ACT_RANK,
					ACT_PROMOTE,
					ACT_DEMOTE,
				} action = ACT_RANK;
			std::string target_name;
			rank new_rank;
				
			std::string arg1 = reader.next ();
			if (sutils::iequals (arg1, "promote"))
				{
					action = ACT_PROMOTE;
					target_name = reader.next ().as_str ();
				}
			else if (sutils::iequals (arg1, "demote"))
				{
					action = ACT_DEMOTE;
					target_name = reader.next ().as_str ();
				}
			else
				{
					target_name = arg1;
					std::string rank_str = reader.next ();
					if (sutils::iequals (rank_str, "up"))
						action = ACT_PROMOTE;
					else if (sutils::iequals (rank_str, "down"))
						action = ACT_DEMOTE;
					else
						{
							try
								{
									new_rank.set (rank_str.c_str (), pl->get_server ().get_groups ());
								}
							catch (const std::exception& str)
								{
									pl->message ("§c * §7Invalid rank§f: §c" + rank_str);
										return;
								}
						}
				}
			
			player *target = nullptr;
			sqlops::player_info pinf;
			{
				soci::session sql (pl->get_server ().sql_pool ());
				if (!sqlops::player_data (sql, target_name.c_str (), pl->get_server (), pinf))
					{
						pl->message ("§c * §7Unknown player§f: §c" + target_name);
						return;
					}
				
				target_name.assign (pinf.name);
				target = pl->get_server ().get_players ().find (target_name.c_str ());
				if (action == ACT_PROMOTE || action == ACT_DEMOTE)
					{
						if (!get_next_rank (pl, pinf.rnk, new_rank, action == ACT_PROMOTE))
							return;
					}
				
				if (!pl->is_op ())
					{
						if (target_name == pl->get_username ())
							{
								pl->message ("§c * §7You cannot change your own rank§c.");
								return;
							}
						else if (pinf.rnk >= pl->get_rank ())
							{
								pl->message ("§c * §7You cannot change the rank of someone higher than you§c.");
								return;
							}
						else if (new_rank >= pl->get_rank ())
							{
								pl->message ("§c * §7You cannot give a player a rank that is higher than yours§c.");
								return;
							}
					}
				
				try
					{
						std::string rank_str;
						new_rank.get_string (rank_str);
						sqlops::modify_player_rank (sql, target_name.c_str (), rank_str.c_str ());
					}
				catch (const std::exception& ex)
					{
						pl->message ("§c * §4Failed to update database§c.");
					}
				
				// update the player's in-game status.
				if (target)
					{
						target->set_rank (new_rank);
						
						if (!quiet)
							{
								group *main = new_rank.main ();
								std::ostringstream ss;
								ss << "§9 > §" << pinf.rnk.main ()->color << pinf.nick <<
									"§7's rank has been set to" //<< (is_vowel (main->name[0]) ? "n" : "")
									<< " §" << main->color << main->name << "§7!";
								pl->get_server ().get_players ().message (ss.str ());
							}
					}
				
				{
					std::ostringstream ss;
					ss << "§7 | The database has been updated.";
					pl->message (ss.str ());
				}
			}
		}
	}
}

