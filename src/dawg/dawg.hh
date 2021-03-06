#pragma once

# include <vector>
# include <algorithm>
# include <cmath>
# include <iostream>
# include <numeric>

# include "dawg_node.hh"


/** 
 * @brief Unused Directed Acyclic Word Graph (was used in a previous version)
 */
template<typename key_type, typename data_type>
class dawg
{
  public:
    dawg();

    void insert(std::string word, data_type data);

    void minimize(long limit);
    
    void minimize2(long limit);

    data_type search(std::string word, unsigned long cost);

    void debug_display();

    void close();

  private:
    unsigned long dawg_node_id;

    dawg_node_ptr<key_type> root;

    std::string prev_word;

    struct unchecked_node
    {
      dawg_node_ptr<key_type> node;
      dawg_node_ptr<key_type> next_node;
      char letter;
    };
    std::vector<unchecked_node> unchecked_nodes;

    std::map<dawg_node_ptr<key_type>, dawg_node_ptr<key_type>> minimized_nodes;//not hunderstood
    
    std::map<char , dawg_node_ptr<key_type>> minimized_nodes2;//not hunderstood

    std::vector<data_type> data;
};

# include "dawg.hxx"
