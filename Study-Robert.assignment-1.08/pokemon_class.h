#ifndef POKEMON_CLASS_H
#define POKEMON_CLASS_H

class pokemon_class{
  public:
    int id;
    int level;
    int hp;
    int attack;
    int defense;
    int speed;
    int specialAttack;
    int specialDefense;
    char gender;
    char shiny;
    char name[30];
};

#endif