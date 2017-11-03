# spell-checker
## Description

This projects aims at doing high performance fuzzy search in large data-sets using custom data structure.
This projects implements a hybrid Trie (previously a DAWG), which is written on RAM and Disk depending on usage

In the end the TextMiningCompiler compiles the whole trie on disk in a file and the TextMiningApp can use that index file to do a fast fuzzy search.

## Build

This project is using the CMake build system.

### In-Source build

```sh
cmake ..
make
```

### Out-of-source build

```sh
mkdir build && cd build
cmake .
cmake . -DCMAKE_BUILD_TYPE=Release
make
```

### Doxygen

```sh
make doc
```

## Dependencies

Boost 1.58.0 is used and its serialization module, to enable seamless serialiaztion of any data structure.

## Design

The compiler was originally designed using a Directed Acyclic Word Graph (DAWG or MA-FSA), but the algorithm was found to use too much memory (> 1Go) and we had trouble merging some nodes.

After this, we designed a Hybrid Trie, that works on RAM and disk, that uses 40Mb RAM, when loaded with the 41M words text file.

That Hybrid Trie has a maximum depth (e.g. 4) and after that, the trie dump the word' suffix on disk. A directory (e.g. "dict.bin/") is created with each file having an id.

When searching, the trie is very fast when the word is in RAM (i.e. size < max_trie_depth) but if the word is too long, we then continue searching on disk which can take a long time. When searching for a word on disk, we do a simple linear search.

Finally, when using the TextMiningCompiler, we only compile the trie in RAM to disk, which is dumped into the dict.bin (for exemple).

In the end, on disk, the Trie looks something like :

-- dict.bin

---- 100

---- 1000

     .... <- skipped a lot here

---- 99999

---- index

To serialize the "index" we use boost::serialize.

## Tests

The following command launches the tests :

```sh
make check
```

It only compares the output with reference given. If it matches, tests are considered passing.

Also, we used profiling tools like perf and heap_track to monitor our cpu and heap usage.

## Failing cases

None to be reported yet.

## Data structures

First, a base trie is created, containing the following elements:

```cpp
    std::string dic;
    unsigned int count_;

    trie_node_ptr<key_type> root_;
    std::map<unsigned int, data_type> words_datatypes;
    unsigned int depth_;
```

- The root is the root of the whole trie.
- The dic variable, is the path where to dump the second part of the trie on disk, when we a word goes beyond a chosen depth.
- The count value, keeps tracks of the number of nodes created and assigns them an id upon creation.
- The depth_ attribute is the max depth after which we dump on the disk.
- The words_datatypes map, is used to store the data that should be in the trie's nodes that are under the max depth_.

On the other hand, we have the trie nodes:

```cpp
    bool  has_child_;
    unsigned int id_;
    std::map<key_type, trie_node_ptr<key_type>> childs_;
    bool  final_node_;
```

- The id_ attribute serves as a unique id for the current node. It is assigned as explained above.
- Childs_ contains all the current node's childs. We use a map to have fast access to these nodes, considering that we don't use much RAM, this is negligeable.
- Final_node_ indicates that the current node is the end of a word. Still, that node can still have children. So this is not akin to a leaf.
- has_child_ points out that that node, had children stored on disk. The current node's id gives the file to open, within the directory's name given by the user (e.g. dict.bin)


##  Fuzzy search


The architecture used here is the following :

- For an exact search, we do a classical search by probing the Trie. But we do it in an iterative fashion.
- For a fuzzy search, we probe the trie, and for each word, we use a Damerau-Levenshtein distance and if the word has a lower or equal distance, we add it to a vector. Finale, we output the result as a JSON object.


##  Possible optimizations


We already use GCC's (almost) maximum possible optimizations that is:

- Compile-time optimizations, with the following flags :"-march=native -Ofast -fprofile-generate -frename-registers -funroll-loops -pg"
- Link-time optimizations : the "-flto" flag enables the use of the Link-time optimizer using gcc-ar
- Run-time Profile-Guided optimizations : Using the flags "-fprofile-generate", gcc can inject code inside the program that enables it to localize hot spots within the execution at run-time. Knowing this, we can recompile the program using the flag "-fprofile-use" and gcc can optimize even more the program. So our programs are compiled in 2 passes each.

The thing is that we have heavy IO on disk usage, we need to be able to reduce that to the maximum if we want way more performance in the future. Although, we only use ~40 Mb, which is pretty low.

Also, we should serialize the Trie while creating it and not after. This avoids having to use boost::serialization which is heavy on cpu time.


##    Using methods closer to the state of the art


We could have used another data structure, like a patricia trie, even though we do dump word suffixes on disk.

Other optimizations include bloom filters, or even implementing a levenshtein automata for the fuzzy search.

Further more, we could use sentences instead of words with frequencies, which will enable us to use particular structures like Hidden Markov Models and enable suggestions based on the word's type and on the language's type.
