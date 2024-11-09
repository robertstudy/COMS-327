#ifndef POKEMON_H
# define POKEMON_H

enum pokemon_stat {
  stat_hp_max,
  stat_atk,
  stat_def,
  stat_spatk,
  stat_spdef,
  stat_speed
};

enum pokemon_gender {
  gender_female,
  gender_male
};

class pokemon {
 private:
  int level;
  int pokemon_index;
  int move_index[4];
  int pokemon_species_index;
  int IV[6];
  int effective_stat[6];
  bool shiny;
  pokemon_gender gender;
  int hp; // added this 1.09
 public:
  pokemon();
  pokemon(int level);
  const char *get_species() const;
  int get_hp_max() const;
  int get_atk() const;
  int get_def() const;
  int get_spatk() const;
  int get_spdef() const;
  int get_speed() const;
  const char *get_gender_string() const;
  bool is_shiny() const;
  const char *get_move(int i) const;
  void set_hp(int hp_setter); // added this 1.09
  int get_hp() const;
  int get_move_priority(int move) const;
  int get_move_accuracy(int move) const;
  int get_level() const;
  int get_move_power(int move) const;
  int get_base_speed() const;
  bool is_stab(int move) const;
};
  
#endif
