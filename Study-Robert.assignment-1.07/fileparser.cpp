#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>

#include "fileparser.h"

pokemon pokemon_arr[1093];
moves moves_arr[845];
pokemon_moves pokemon_moves_arr[528239];
pokemon_species species_arr[899];
experience experience_arr[601];
type_names type_names_arr[194];
pokemon_stats pokemon_stats_arr[6553];
stats stats_arr[8];
pokemon_types pokemon_types_arr[1676];

/*
Your program should look in at least 2 and optionally 3 places for
the files, only failing if none of those locations contain the database. Your program should first look under
/share/cs327. Failing that, it should look under $HOME/.poke327/. And, optionally, it may look in
a third place (which is NOT UNDER YOUR SOURCE TREE) of your choosing. For the second location,
use getenv() to resolve the value of the HOME environment variable.
*/

void parse_pokemon(){ 
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 1092; i++){
        pokemon_arr[i].id = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].id = pokemon_arr[i].id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].id = -1;
        }
        param_found = 0;
        csv_file.get();
        int j = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].identifier[j] = csv_file.get();
            j++;
        }
        param_found = 0;
        csv_file.get();
        pokemon_arr[i].species_id = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].species_id = pokemon_arr[i].species_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].species_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_arr[i].height = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].height = pokemon_arr[i].height * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].height = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_arr[i].weight = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].weight = pokemon_arr[i].weight * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].weight = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_arr[i].base_experience = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].base_experience = pokemon_arr[i].base_experience * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].base_experience = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_arr[i].order = 0;
        while(csv_file.peek() != ','){
            pokemon_arr[i].order = csv_file.get() - 48;
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_arr[i].order = -1;
        }

        getline(csv_file, s);
        param_found = 0;
        pokemon_arr[i].is_default = 1;

        printf("%d %s %d %d %d %d %d %d\n", pokemon_arr[i].id, pokemon_arr[i].identifier, pokemon_arr[i].species_id, pokemon_arr[i].height, pokemon_arr[i].weight, pokemon_arr[i].base_experience, pokemon_arr[i].order, pokemon_arr[i].is_default);
    }
    csv_file.close();
}

void parse_moves(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/moves.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/moves.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 844; i++){
        moves_arr[i].id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].id = moves_arr[i].id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].id = -1;
        }
        csv_file.get();
        param_found = 0;
        int j = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].identifier[j] += csv_file.get();
            j++;
        }
        csv_file.get();
        moves_arr[i].generation_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].generation_id = moves_arr[i].generation_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].generation_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].type_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].type_id = moves_arr[i].type_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].type_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].power = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].power = moves_arr[i].power * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].power = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].pp = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].pp = moves_arr[i].pp * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].pp = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].accuracy = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].accuracy = moves_arr[i].accuracy * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].accuracy = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].priority = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].priority = moves_arr[i].priority * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].priority = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].target_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].target_id = moves_arr[i].target_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].target_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].damage_class_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].damage_class_id = moves_arr[i].damage_class_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].damage_class_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].effect_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].effect_id = moves_arr[i].effect_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].effect_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].effect_chance = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].effect_chance = moves_arr[i].effect_chance * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].effect_chance = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].contest_type_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].contest_type_id = moves_arr[i].contest_type_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].contest_type_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].contest_effect_id = 0;
        while(csv_file.peek() != ','){
            moves_arr[i].contest_effect_id = moves_arr[i].contest_effect_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].contest_effect_id = -1;
        }
        csv_file.get();
        param_found = 0;
        moves_arr[i].super_contest_effect_id = 0;
        while(csv_file.peek() != '\n'){
            moves_arr[i].super_contest_effect_id = moves_arr[i].super_contest_effect_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            moves_arr[i].super_contest_effect_id = -1;
        }
        csv_file.get();
        printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n", moves_arr[i].id, moves_arr[i].identifier, moves_arr[i].generation_id, moves_arr[i].type_id, moves_arr[i].power, moves_arr[i].pp, moves_arr[i].accuracy, moves_arr[i].priority, moves_arr[i].target_id, moves_arr[i].damage_class_id, moves_arr[i].effect_id, moves_arr[i].effect_chance, moves_arr[i].contest_type_id, moves_arr[i].contest_effect_id, moves_arr[i].super_contest_effect_id);
    }
    csv_file.close();
}

void parse_pokemon_moves(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_moves.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_moves.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 528238; i++){
        pokemon_moves_arr[i].pokemon_id = 0;
        while(csv_file.peek() != ','){
            pokemon_moves_arr[i].pokemon_id = pokemon_moves_arr[i].pokemon_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].pokemon_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_moves_arr[i].version_group_id = 0;
        while(csv_file.peek() != ','){
            pokemon_moves_arr[i].version_group_id = pokemon_moves_arr[i].version_group_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].version_group_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_moves_arr[i].move_id = 0;
        while(csv_file.peek() != ','){
            pokemon_moves_arr[i].move_id = pokemon_moves_arr[i].move_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].move_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_moves_arr[i].pokemon_move_method = 0;
        while(csv_file.peek() != ','){
            pokemon_moves_arr[i].pokemon_move_method = pokemon_moves_arr[i].pokemon_move_method * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].pokemon_move_method = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_moves_arr[i].level = 0;
        while(csv_file.peek() != ','){
            pokemon_moves_arr[i].level = pokemon_moves_arr[i].level * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].level = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_moves_arr[i].order = 0;
        while(csv_file.peek() != '\n'){
            pokemon_moves_arr[i].order = pokemon_moves_arr[i].order * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_moves_arr[i].order = -1;
        }
        csv_file.get();
        param_found = 0;
        printf("%d %d %d %d %d %d \n", pokemon_moves_arr[i].pokemon_id, pokemon_moves_arr[i].version_group_id, pokemon_moves_arr[i].move_id, pokemon_moves_arr[i].pokemon_move_method, pokemon_moves_arr[i].level, pokemon_moves_arr[i].order);
    }
    csv_file.close();
}

void parse_pokemon_species(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_species.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_species.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 898; i++){
        species_arr[i].id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].id = species_arr[i].id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].id = -1;
        }
        csv_file.get();
        param_found = 0;
        int j = 0;
        while(csv_file.peek() != ','){
            species_arr[i].identifier[j] = csv_file.get();
            j++;
        }
        csv_file.get();
        species_arr[i].generation_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].generation_id = species_arr[i].generation_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].generation_id = -1;
        } 
        csv_file.get();
        param_found = 0;
        species_arr[i].evolves_from_species_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].evolves_from_species_id = species_arr[i].evolves_from_species_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].evolves_from_species_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].evolution_chain_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].evolution_chain_id = species_arr[i].evolution_chain_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].evolution_chain_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].color_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].color_id = species_arr[i].color_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].color_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].shape_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].shape_id = species_arr[i].shape_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].shape_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].habitat_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].habitat_id = species_arr[i].habitat_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].habitat_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].gender_rate = 0;
        while(csv_file.peek() != ','){
            species_arr[i].gender_rate = species_arr[i].gender_rate * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].gender_rate = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].capture_rate = 0;
        while(csv_file.peek() != ','){
            species_arr[i].capture_rate = species_arr[i].capture_rate * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].capture_rate = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].base_happiness = 0;
        while(csv_file.peek() != ','){
            species_arr[i].base_happiness = species_arr[i].base_happiness * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].base_happiness = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].is_baby = 0;
        while(csv_file.peek() != ','){
            species_arr[i].is_baby = species_arr[i].is_baby * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].is_baby = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].hatch_counter = 0;
        while(csv_file.peek() != ','){
            species_arr[i].hatch_counter = species_arr[i].hatch_counter * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].hatch_counter = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].has_gender_differences = 0;
        while(csv_file.peek() != ','){
            species_arr[i].has_gender_differences = species_arr[i].has_gender_differences * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].has_gender_differences = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].growth_rate_id = 0;
        while(csv_file.peek() != ','){
            species_arr[i].growth_rate_id = species_arr[i].growth_rate_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].growth_rate_id = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].forms_switchable = 0;
        while(csv_file.peek() != ','){
            species_arr[i].forms_switchable = species_arr[i].forms_switchable * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].forms_switchable = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].is_legendary = 0;
        while(csv_file.peek() != ','){
            species_arr[i].is_legendary = species_arr[i].is_legendary * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].is_legendary = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].is_mythical = 0;
        while(csv_file.peek() != ','){
            species_arr[i].is_mythical = species_arr[i].is_mythical * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].is_mythical = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].order = 0;
        while(csv_file.peek() != ','){
            species_arr[i].order = species_arr[i].order * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].order = -1;
        }
        csv_file.get();
        param_found = 0;
        species_arr[i].conquest_order = 0;
        while(csv_file.peek() != '\n'){
            species_arr[i].conquest_order = species_arr[i].conquest_order * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            species_arr[i].conquest_order = -1;
        }
        csv_file.get();
        param_found = 0;
        printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", species_arr[i].id, species_arr[i].identifier, species_arr[i].generation_id, species_arr[i].evolves_from_species_id, species_arr[i].evolution_chain_id, species_arr[i].color_id, species_arr[i].shape_id, species_arr[i].habitat_id, species_arr[i].gender_rate, species_arr[i].capture_rate, species_arr[i].base_happiness, species_arr[i].is_baby, species_arr[i].hatch_counter, species_arr[i].has_gender_differences, species_arr[i].growth_rate_id, species_arr[i].forms_switchable, species_arr[i].is_legendary, species_arr[i].is_mythical, species_arr[i].order, species_arr[i].conquest_order);
    }
    csv_file.close();
}

void parse_experience(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/experience.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/experience.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 600; i++){
        experience_arr[i].growth_rate_id = 0;
        while(csv_file.peek() != ','){
            experience_arr[i].growth_rate_id = experience_arr[i].growth_rate_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            experience_arr[i].growth_rate_id = -1;
        }
        csv_file.get();
        param_found = 0;
        experience_arr[i].level = 0;
        while(csv_file.peek() != ','){
            experience_arr[i].level = experience_arr[i].level * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            experience_arr[i].level = -1;
        }
        csv_file.get();
        param_found = 0;
        experience_arr[i].experience = 0;
        while(csv_file.peek() != '\n'){
            experience_arr[i].experience = experience_arr[i].experience * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            experience_arr[i].experience = -1;
        }
        csv_file.get();
        param_found = 0; 
        printf("%d %d %d\n", experience_arr[i].growth_rate_id, experience_arr[i].level, experience_arr[i].experience);       
    }
    csv_file.close();
}

void parse_type_names(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/type_names.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/type_names.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 194; i++){
        type_names_arr[i].type_id = 0;
        while(csv_file.peek() != ','){
            type_names_arr[i].type_id = type_names_arr[i].type_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            type_names_arr[i].type_id = -1;
        }
        csv_file.get();
        param_found = 0;
        type_names_arr[i].local_language_id = 0;
        while(csv_file.peek() != ','){
            type_names_arr[i].local_language_id = type_names_arr[i].local_language_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            type_names_arr[i].local_language_id = -1;
        }
        csv_file.get();
        param_found = 0;
        int j = 0;
        while(csv_file.peek() != '\n'){
            type_names_arr[i].name[j] = csv_file.get();
            j++;
        }
        csv_file.get();
        printf("%d %d %s\n", type_names_arr[i].type_id, type_names_arr[i].local_language_id, type_names_arr[i].name);
    }
    csv_file.close();
}

void parse_pokemon_stats(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_stats.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_stats.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 6552; ++i){
        pokemon_stats_arr[i].pokemon_id = 0;
        while(csv_file.peek() != ','){
            pokemon_stats_arr[i].pokemon_id = pokemon_stats_arr[i].pokemon_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_stats_arr[i].pokemon_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_stats_arr[i].stat_id = 0;
        while(csv_file.peek() != ','){
            pokemon_stats_arr[i].stat_id = pokemon_stats_arr[i].stat_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_stats_arr[i].stat_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_stats_arr[i].base_stat = 0;
        while(csv_file.peek() != ','){
            pokemon_stats_arr[i].base_stat = pokemon_stats_arr[i].base_stat * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_stats_arr[i].base_stat = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_stats_arr[i].effort = 0;
        while(csv_file.peek() != '\n'){
            pokemon_stats_arr[i].effort = pokemon_stats_arr[i].effort * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_stats_arr[i].effort = -1;
        }
        csv_file.get();
        param_found = 0;
        printf("%d %d %d %d\n", pokemon_stats_arr[i].pokemon_id, pokemon_stats_arr[i].stat_id, pokemon_stats_arr[i].base_stat, pokemon_stats_arr[i].effort);
    }
    csv_file.close();
}

void parse_stats(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/stats.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/stats.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 8; i++){
        param_found = 0;
        
        while(csv_file.peek() != ','){
            stats_arr[i].id = stats_arr[i].id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            stats_arr[i].id = -1;
        }
        csv_file.get();
        param_found = 0;
        while(csv_file.peek() != ','){
            stats_arr[i].damage_class_id = stats_arr[i].damage_class_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            stats_arr[i].damage_class_id = -1;
        }
        csv_file.get();
        param_found = 0;
        int j = 0;
        while(csv_file.peek() != ','){
            stats_arr[i].identifier[j] = csv_file.get();
            j++;
        }
        csv_file.get();
        stats_arr[i].is_battle_only = 0;
        while(csv_file.peek() != ','){
            stats_arr[i].is_battle_only = stats_arr[i].is_battle_only * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            stats_arr[i].is_battle_only = -1;
        }
        csv_file.get();
        param_found = 0;
        stats_arr[i].game_index = 0;
        while(csv_file.peek() != '\n'){
            stats_arr[i].game_index = stats_arr[i].game_index * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            stats_arr[i].game_index = -1;
        }
        csv_file.get();
        param_found = 0;

        printf("%d %d %s %d %d\n", stats_arr[i].id, stats_arr[i].damage_class_id, stats_arr[i].identifier, stats_arr[i].is_battle_only, stats_arr[i].game_index);
    }
    csv_file.close();
}

void parse_pokemon_types(){
    std::ifstream csv_file;
    csv_file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_types.csv");
    if(!csv_file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_types.csv";
        csv_file.open(home);
        if(!csv_file){
            printf("File path not found");
            return;
        }
    }
    std::string s;
    getline(csv_file, s);
    int param_found;
    int i;
    for(i = 0; i < 1675; i++){
        pokemon_types_arr[i].pokemon_id = 0;
        while(csv_file.peek() != ','){
            pokemon_types_arr[i].pokemon_id = pokemon_types_arr[i].pokemon_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_types_arr[i].pokemon_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_types_arr[i].type_id = 0;
        while(csv_file.peek() != ','){
            pokemon_types_arr[i].type_id = pokemon_types_arr[i].type_id * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_types_arr[i].type_id = -1;
        }
        csv_file.get();
        param_found = 0;
        pokemon_types_arr[i].slot = 0;
        while(csv_file.peek() != '\n'){
            pokemon_types_arr[i].slot = pokemon_types_arr[i].slot * 10 + (csv_file.get() - 48);
            param_found = 1;
        }
        if(param_found != 1){
            pokemon_types_arr[i].slot = -1;
        }
        csv_file.get();
        param_found = 0;
        printf("%d %d %d\n", pokemon_types_arr[i].pokemon_id, pokemon_types_arr[i].type_id, pokemon_types_arr[i].slot);
    }
    csv_file.close();
}