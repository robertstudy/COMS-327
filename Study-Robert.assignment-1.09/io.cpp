#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <cstring>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"

#define TRAINER_LIST_FIELD_WIDTH 46
 
typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_bailey:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] ==
             DIJKSTRA_PATH_MAX                                            ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[TRAINER_LIST_FIELD_WIDTH],
                                   uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(npc **c, uint32_t count)
{
  uint32_t i;
  char (*s)[TRAINER_LIST_FIELD_WIDTH]; /* pointer to array of 40 char */

  s = (char (*)[TRAINER_LIST_FIELD_WIDTH]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], TRAINER_LIST_FIELD_WIDTH, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], TRAINER_LIST_FIELD_WIDTH, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = dynamic_cast <npc *> (world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "The Pokemart has replenished your items back to 5 each.");
  refresh();

  // Restore the player's items (max of 5)
  int i;
  for(i = 0; i < 3; i++) {
    world.pc.items[i] = 5;
  }

  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "The Pokecenter has restored all of your Pokemon to full health.");
  refresh();

  // Heal all the player's Pokemon
  int i;
  for(i = 0; i < 6 && world.pc.buddy[i]; i++) {
    world.pc.buddy[i]->set_hp(world.pc.buddy[i]->get_hp_max());
  }

  getch();
}

int io_use_bag_item(int index) {

  char use_bag_item_on;

  if(index == 0) { // index 0 is the pokeballs
    if(world.pc.items[0] > 0) { // Check if player has pokeballs.
      if(world.pc.buddy[5] != NULL) {
        return 0;
      }
      int i;
      for(i = 1; i < 6; i++) {
        if(world.pc.buddy[i] == NULL) {
          world.pc.items[0]--;
          return i;
        }
      }
    }else {
      return -1;
    }
  }

  else if (index == 1) {  // index 1 is the potions
    if (world.pc.items[1] > 0) { // Check if player has potions.
        mvprintw(11, 1, "Which Pokemon would you like to heal?");
        mvprintw(12, 2, "[c] cancel/none");

        for (int i = 0; i < 6; ++i) {
            if (world.pc.buddy[i] != NULL) {
                mvprintw(13 + i, 2, "[%d] %s", i + 1, world.pc.buddy[i]->get_species());
            }
        }

        use_bag_item_on = getch();

        if (use_bag_item_on >= '1' && use_bag_item_on <= '6') {
            int pokemonIndex = use_bag_item_on - '1';
            if (world.pc.buddy[pokemonIndex] != NULL && world.pc.buddy[pokemonIndex]->get_hp() < world.pc.buddy[pokemonIndex]->get_hp_max()) {
                world.pc.buddy[pokemonIndex]->set_hp(std::min((world.pc.buddy[pokemonIndex]->get_hp() + 20), world.pc.buddy[pokemonIndex]->get_hp_max()));
                world.pc.items[1]--; // Decrease potion count
                return 0;
            } else {
                return 1; 
            }
        } else if (use_bag_item_on == 'c') {
            return 1; 
        }
    } else {
        return 1; 
    }
  }

  
  else { // index 2 is the revives
    if (world.pc.items[2] > 0) { // Check if player has revive.
        mvprintw(11, 1, "Which Pokemon would you like to revive?");
        mvprintw(12, 2, "[c] cancel/none");

        for (int i = 0; i < 6; ++i) {
            if (world.pc.buddy[i] != NULL) {
                mvprintw(13 + i, 2, "[%d] %s", i + 1, world.pc.buddy[i]->get_species());
            }
        }

        use_bag_item_on = getch();

        if (use_bag_item_on >= '1' && use_bag_item_on <= '6') {
            int pokemonIndex = use_bag_item_on - '1';
            if (world.pc.buddy[pokemonIndex] != NULL && world.pc.buddy[pokemonIndex]->get_hp() <= 0) {
                world.pc.buddy[pokemonIndex]->set_hp(world.pc.buddy[pokemonIndex]->get_hp_max() / 2);
                world.pc.items[2]--; 
                return 0;
            } else {
                return 1;
            }
        } else if (use_bag_item_on == 'c') {
            return 1;
        }
    } else {
        return 1;
    }
  }

  return 0;
}

void io_attack(pokemon *opponent, pokemon *curr_pokemon, int move) {
  
  int opponent_priority = opponent->get_move_priority(0);
    int opponent_speed = opponent->get_speed();
    int curr_priority = curr_pokemon->get_move_priority(move);
    int curr_speed = curr_pokemon->get_speed();
    bool pc_moves_first = (curr_priority > opponent_priority) || (curr_priority == opponent_priority && curr_speed > opponent_speed) || (curr_priority == opponent_priority && curr_speed == opponent_speed && (rand() % 2 == 0));
    int damage;

    if (pc_moves_first) {
        if ((rand() % 100) < curr_pokemon->get_move_accuracy(move)) {

            double random = 0.85 + (rand() % 16) / 100.0;
            double critical = 1.0;
            double type = 1.0;
            double stab = 1.0;

            /*if (opponent->is_stab(move) == true) {
              stab = 1.5;
            }

            if (opponent->get_base_speed() > 0 && (rand() % 255) < opponent->get_base_speed() / 2) {
              critical = 1.5;
            }*/
            
            double damage_multi = critical * type * stab * random;

            damage = ((((((2 * curr_pokemon->get_level()) / 5) + 2) * curr_pokemon->get_move_power(move) * (curr_pokemon->get_atk() / opponent->get_def())) / 50) + 2) * damage_multi;
            opponent->set_hp(std::max((opponent->get_hp() - damage), 0));
        }
    } else {
        if ((rand() % 100) < opponent->get_move_accuracy(0)) {

            double random = 0.85 + (rand() % 16) / 100.0;
            double critical = 1.0;
            double type = 1.0;
            double stab = 1.0;

            /*if (opponent->is_stab(move) == true) {
              stab = 1.5;
            }

            if (opponent->get_base_speed() > 0 && (rand() % 255) < opponent->get_base_speed() / 2) {
              critical = 1.5;
            }*/
            
            double damage_multi = critical * type * stab * random;

            damage = ((((((2 * opponent->get_level()) / 5) + 2) * opponent->get_move_power(0) * (opponent->get_atk() / curr_pokemon->get_def())) / 50) + 2) * damage_multi;
            curr_pokemon->set_hp(std::max((curr_pokemon->get_hp() - damage), 0));
        }
    }
}

int io_switch_pokemon(pokemon **pcPoke) {
    char switch_pokemon;

    mvprintw(11, 1, "Which Pokemon do you want to switch into battle?");
    mvprintw(12, 2, "[c] cancel/none");

    for (int i = 0; i < 6; ++i) {
        if (world.pc.buddy[i] != NULL && world.pc.buddy[i]->get_hp() > 0) {
            mvprintw(13 + i, 2, "[%d] %s", i + 1, world.pc.buddy[i]->get_species());
        }
    }

    switch (switch_pokemon = getch()) {
        case 'c':
            return 1;
        default:
            int index = switch_pokemon - '1';
            if (index >= 0 && index < 6 && world.pc.buddy[index] != NULL && world.pc.buddy[index]->get_hp() > 0 && *pcPoke != world.pc.buddy[index]) {
                *pcPoke = world.pc.buddy[index];
                return 0;
            } else {
                return 1;
            }
    }
}

void io_battle(character *aggressor, character *defender) {

    npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
    bool battle_continue = true;
    int turn_not_consumed = 1;
    int j;
    int npc_poke_index = 0;
    pokemon *npcPoke = n->buddy[npc_poke_index];

    pokemon *pcPoke;
    for (j = 0; j < 6; j++) {
        if (world.pc.buddy[j]->get_hp() != 0) {
            pcPoke = world.pc.buddy[j];
            break;
        }
    }

    do {
        do {
            clear();

            mvprintw(0, 1, "Your Pokemon: %s", pcPoke->get_species());
            mvprintw(1, 1, "HP: %d", pcPoke->get_hp());
            mvprintw(0, 39, "Opponent's Pokemon: %s", npcPoke->get_species());
            mvprintw(1, 39, "HP: %d", npcPoke->get_hp());
            mvprintw(3, 6, "Select an action: make move, use item, or switch pokemon.");

            // Display Player's moves
            mvprintw(5, 1, "Moves:");
            for (int i = 0; i < 4; ++i) {
                if (strcmp(pcPoke->get_move(i), "") != 0) {
                    mvprintw(6 + i, 2, "[%d] %s", i + 1, pcPoke->get_move(i));
                }
            }

            // Display Player's items
            mvprintw(5, 25, "Items:");
            mvprintw(6, 26, "[p] Potion (x%d)", world.pc.items[1]);
            mvprintw(7, 26, "[r] Revive (x%d)", world.pc.items[2]);
            mvprintw(11, 1, "[s] Switch Pokemon");

            char battleInput = getch();

            switch (battleInput) {
                case '1':
                case '2':
                case '3':
                case '4':
                    io_attack(npcPoke, pcPoke, battleInput - '1');
                    turn_not_consumed = 0;
                    break;
                case 'p':
                    turn_not_consumed = io_use_bag_item(1);
                    if (turn_not_consumed == 0) {
                        int damage;
                        if ((rand() % 100) < npcPoke->get_move_accuracy(0)) {

                            double random = 0.85 + (rand() % 16) / 100.0;
                            double critical = 1.0;
                            double type = 1.0;
                            double stab = 1.0;

                            double damage_multi = critical * type * stab * random;
                            damage = ((((((2 * npcPoke->get_level()) / 5) + 2) * npcPoke->get_move_power(0) * (npcPoke->get_atk() / pcPoke->get_def())) / 50) + 2) * damage_multi;
                            pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
                        }
                    }
                    break;
                case 'r':
                    turn_not_consumed = io_use_bag_item(2);
                    if (turn_not_consumed == 0) {
                        int damage;
                        if ((rand() % 100) < npcPoke->get_move_accuracy(0)) {
                            double random = 0.85 + (rand() % 16) / 100.0;
                            double critical = 1.0;
                            double type = 1.0;
                            double stab = 1.0;

                            double damage_multi = critical * type * stab * random;
                            damage = ((((((2 * npcPoke->get_level()) / 5) + 2) * npcPoke->get_move_power(0) * (npcPoke->get_atk() / pcPoke->get_def())) / 50) + 2) * damage_multi;
                            pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
                        }
                    }
                    break;
                case 's':
                    turn_not_consumed = io_switch_pokemon(&pcPoke);
                    if (turn_not_consumed == 0) {
                        int damage;
                        if ((rand() % 100) < npcPoke->get_move_accuracy(0)) {
                            double random = 0.85 + (rand() % 16) / 100.0;
                            double critical = 1.0;
                            double type = 1.0;
                            double stab = 1.0;

                            double damage_multi = critical * type * stab * random;
                            damage = ((((((2 * npcPoke->get_level()) / 5) + 2) * npcPoke->get_move_power(0) * (npcPoke->get_atk() / pcPoke->get_def())) / 50) + 2) * damage_multi;
                            pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
                        }
                    }
                    break;
                default:
                    turn_not_consumed = 1;
            }
        } while (turn_not_consumed);

        if (npcPoke->get_hp() == 0) {
            ++npc_poke_index;
            if (npc_poke_index == 6 || n->buddy[npc_poke_index] == NULL) {
                n->defeated = 1;
                if (n->ctype == char_hiker || n->ctype == char_rival) {
                    n->mtype = move_wander;
                }
                battle_continue = false;
            } else {
                npcPoke = n->buddy[npc_poke_index];
            }
        }

        if (pcPoke->get_hp() == 0) {
            int k;
            for (k = 0; k < 6; k++) {
                if (world.pc.buddy[k] == NULL) {
                    world.quit = 2;
                    battle_continue = false;
                    break;
                }
                if (world.pc.buddy[k]->get_hp() != 0) {
                    pcPoke = world.pc.buddy[k];
                    break;
                }
            }
        }
    } while (battle_continue);
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *> (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if ((dynamic_cast<npc *>
                (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]))) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      DIJKSTRA_PATH_MAX) {
    return 1;
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_gate &&
      dest[dim_y] != world.pc.pos[dim_y]                       &&
      dest[dim_x] != world.pc.pos[dim_x]) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_bag() {
  bool done = false;

  do {
    int turn_not_consumed = 0;
    do {
      clear();

      // Items
      mvprintw(0, 1, "Items in your bag:");
      mvprintw(1, 2, "[X] Pokeballs - %d", world.pc.items[0]);
      mvprintw(2, 2, "[p] Potions - %d", world.pc.items[1]);
      mvprintw(3, 2, "[r] Revives - %d", world.pc.items[2]);

      // Pokemon
      mvprintw(0, 25, "Your Pokemon:");
      mvprintw(1, 26, "[1] %s - HP: %d/%d", world.pc.buddy[0]->get_species(), world.pc.buddy[0]->get_hp(), world.pc.buddy[0]->get_hp_max());
      if(world.pc.buddy[1] != NULL) {
        mvprintw(2, 26, "[2] %s - HP: %d/%d", world.pc.buddy[1]->get_species(), world.pc.buddy[1]->get_hp(), world.pc.buddy[1]->get_hp_max());
      }
      if(world.pc.buddy[2] != NULL) {
        mvprintw(3, 26, "[3] %s - HP: %d/%d", world.pc.buddy[2]->get_species(), world.pc.buddy[2]->get_hp(), world.pc.buddy[2]->get_hp_max());
      }
      if(world.pc.buddy[3] != NULL) {
        mvprintw(4, 26, "[4] %s - HP: %d/%d", world.pc.buddy[3]->get_species(), world.pc.buddy[3]->get_hp(), world.pc.buddy[3]->get_hp_max());
      }
      if(world.pc.buddy[4] != NULL) {
        mvprintw(5, 26, "[5] %s - HP: %d/%d", world.pc.buddy[4]->get_species(), world.pc.buddy[4]->get_hp(), world.pc.buddy[4]->get_hp_max());
      }
      if(world.pc.buddy[5] != NULL) {
        mvprintw(6, 26, "[6] %s - HP: %d/%d", world.pc.buddy[5]->get_species(), world.pc.buddy[5]->get_hp(), world.pc.buddy[5]->get_hp_max());
      }

      // Leave
      mvprintw(22, 1, "[e] Exit bag");

      char item_use;

      switch(item_use = getch()) {
        case 'p':
          turn_not_consumed = io_use_bag_item(1);
          break;
        case 'r':
          turn_not_consumed = io_use_bag_item(2);
          break;
        case 'e':
          done = true;
          turn_not_consumed = 0;
          break;
        default:
          turn_not_consumed = 1;
          break;
      }
    } while(turn_not_consumed);
  } while(!done);

  return;
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
   case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;    
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

void io_encounter_pokemon(){

  pokemon *wildPoke = new pokemon();
  bool battle_continue = true;
  int turn_not_consumed = 1;
  int j;

  pokemon *pcPoke;
  for(j = 0; j < 6; j++) {
    if(world.pc.buddy[j]->get_hp() != 0){
      pcPoke = world.pc.buddy[j];
      break;
    }
  }

  do {
    do {
      clear();

      mvprintw(0, 1, "Your Pokemon: %s", pcPoke->get_species());
      mvprintw(1, 1, "HP: %d", pcPoke->get_hp());
      mvprintw(0, 39, "Wild Pokemon: %s", wildPoke->get_species());
      mvprintw(1, 39, "HP: %d", wildPoke->get_hp());
      mvprintw(3, 12, "SELECT AN ACTION!");

      // Player's moves
      mvprintw(5, 1, "Moves:");
      mvprintw(6, 2, "[1] %s", pcPoke->get_move(0));
      if(strcmp(pcPoke->get_move(1), "") != 0) {
        mvprintw(7, 2, "[2] %s", pcPoke->get_move(1));
      }
      if(strcmp(pcPoke->get_move(2), "") != 0) {
        mvprintw(8, 2, "[3] %s", pcPoke->get_move(2));
      }
      if(strcmp(pcPoke->get_move(3), "") != 0) {
        mvprintw(9, 2, "[4] %s", pcPoke->get_move(3));
      }
      // Player's items
      mvprintw(5, 25, "Items:");
      mvprintw(6, 26, "[b] Pokeball (x%d)", world.pc.items[0]);
      mvprintw(7, 26, "[p] Potion (x%d)", world.pc.items[1]);
      mvprintw(8, 26, "[r] Revive (x%d)", world.pc.items[2]);
      mvprintw(11, 1, "[s] Switch Pokemon");
      mvprintw(12, 1, "[f] Flee");

      char encounterInput;
      int catch_result;

      switch(encounterInput = getch()) {
        case '1':
          io_attack(wildPoke, pcPoke, 0);
          turn_not_consumed = 0;
          break;
        case '2':
          if(strcmp(pcPoke->get_move(1), "") != 0) {
            io_attack(wildPoke, pcPoke, 1);
            turn_not_consumed = 0;
          }
          else {
            turn_not_consumed = 1;
          }
          break;
        case '3':
          if(strcmp(pcPoke->get_move(2), "") != 0) {
            io_attack(wildPoke, pcPoke, 2);
            turn_not_consumed = 0;
          }
          else {
            turn_not_consumed = 1;
          }
          break;
        case '4':
          if(strcmp(pcPoke->get_move(3), "") != 0) {
            io_attack(wildPoke, pcPoke, 3);
            turn_not_consumed = 0;
          }
          else {
            turn_not_consumed = 1;
          }
          break;
        case 'b':
          catch_result = io_use_bag_item(0);
          if(catch_result < 0) {
            turn_not_consumed = 1;
          }
          else if(catch_result == 0) {
            turn_not_consumed = 0;
            delete wildPoke;
            battle_continue = false;
          }
          else {
            world.pc.buddy[catch_result] = wildPoke;
            turn_not_consumed = 0;
            battle_continue = false;
          }
          break;
        case 'p':
          turn_not_consumed = io_use_bag_item(1);
          if(turn_not_consumed == 0) {
            int damage;
            if((rand() % 100) < wildPoke->get_move_accuracy(0)) {
              damage = ((((((2 * wildPoke->get_level()) / 5) + 2) * wildPoke->get_move_power(0) * (wildPoke->get_atk() / pcPoke->get_def())) / 50) + 2);

              pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
            }
          }
          break;
        case 'r':
          turn_not_consumed = io_use_bag_item(2);
          if(turn_not_consumed == 0) {
            int damage;
            if((rand() % 100) < wildPoke->get_move_accuracy(0)) {
              damage = ((((((2 * wildPoke->get_level()) / 5) + 2) * wildPoke->get_move_power(0) * (wildPoke->get_atk() / pcPoke->get_def())) / 50) + 2);

              pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
            }
          }
          break;
        case 's':
          turn_not_consumed = io_switch_pokemon(&pcPoke);
          if(turn_not_consumed == 0) {
            int damage;
            if((rand() % 100) < wildPoke->get_move_accuracy(0)) {
              damage = ((((((2 * wildPoke->get_level()) / 5) + 2) * wildPoke->get_move_power(0) * (wildPoke->get_atk() / pcPoke->get_def())) / 50) + 2);

              pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
            }
          }
          break;
        case 'f':
          if((rand() % 100) < 95) { // 95% chance to successfully flee
            battle_continue = false;
          }
          else {
            int damage;
            if((rand() % 100) < wildPoke->get_move_accuracy(0)) {
              damage = ((((((2 * wildPoke->get_level()) / 5) + 2) * wildPoke->get_move_power(0) * (wildPoke->get_atk() / pcPoke->get_def())) / 50) + 2);

              pcPoke->set_hp(std::max((pcPoke->get_hp() - damage), 0));
            }
          }
          turn_not_consumed = 0;
          break;
        default:
          turn_not_consumed = 1;
          break;
      }
    } while (turn_not_consumed);
    
    if(wildPoke->get_hp() == 0) {
      delete wildPoke;
      battle_continue = false;
    }

    if(pcPoke->get_hp() == 0) {
      int k;

      for(k = 0; k <= 6; k++) {
        if(k == 6 || world.pc.buddy[k] == NULL) {
          world.quit = 2;
          battle_continue = false;
          break;
        }

        if(world.pc.buddy[k]->get_hp() != 0){
          pcPoke = world.pc.buddy[k];
          break;
        }
      }
    }

  } while(battle_continue);
}

void io_choose_starter()
{
  class pokemon *choice[3];
  int i;
  bool again = true;
  
  choice[0] = new class pokemon();
  choice[1] = new class pokemon();
  choice[2] = new class pokemon();

  echo();
  curs_set(1);
  do {
    mvprintw( 4, 20, "Before you are three Pokemon, each of");
    mvprintw( 5, 20, "which wants absolutely nothing more");
    mvprintw( 6, 20, "than to be your best buddy forever.");
    mvprintw( 8, 20, "Unfortunately for them, you may only");
    mvprintw( 9, 20, "pick one.  Choose wisely.");
    mvprintw(11, 20, "   1) %s", choice[0]->get_species());
    mvprintw(12, 20, "   2) %s", choice[1]->get_species());
    mvprintw(13, 20, "   3) %s", choice[2]->get_species());
    mvprintw(15, 20, "Enter 1, 2, or 3: ");

    refresh();
    i = getch();

    if (i == '1' || i == '2' || i == '3') {
      world.pc.buddy[0] = choice[(i - '0') - 1];
      delete choice[(i - '0') % 3];
      delete choice[((i - '0') + 1) % 3];
      again = false;
    }
  } while (again);
  noecho();
  curs_set(0);
}
