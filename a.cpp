#include <array>
#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
// hardcoded delay for propagation and races
#define delay usleep(5000);

enum LogicTypes
{
    AND,
    OR,
    NOT,
    XOR,
    NAND,
    NOR
};
// vector of std::function
std::vector<std::function<bool(std::vector<bool>)>> functions{
    // AND
    [](const std::vector<bool> & v) {
        return v[0] == 1 && std::adjacent_find(v.begin(), v.end(), std::not_equal_to<>()) == v.end();
    },
    // OR
    [](const std::vector<bool> & v) { return std::find(v.begin(), v.end(), 1) != v.end(); }};
std::vector<std::string> inputs;
std::vector<bool> outer_inputs;
std::vector<uint> gate_types;
std::vector<std::string> gate_names;
std::unordered_map<std::string, uint> name_to_idx;
// output of gate_name, indices are parallel
std::vector<bool> output_bits;
// inputs of gate_names, indices are parallel, each subvec holds the indices of what it needs.
std::vector<std::vector<int>> input_locs;
bool ready;
std::condition_variable cv;
std::mutex m;
std::string output_gate;
void function_caller(std::function<bool(std::vector<bool>)> F, std::vector<int> locs, uint cLoc)
{
    std::vector<bool> caller;
    while (true)
    {
        if (ready)
            break;
        std::unique_lock<std::mutex> lck(m);
        for (auto i : locs)
        {
            if (i < 0)
            {
                caller.push_back(outer_inputs[(i + 1) * -1]);
            }
            else
            {
                caller.push_back(output_bits[i]);
            }
        }
        if (caller.size() > 6)
        {
            caller[0] = caller[0];
        }
        output_bits[cLoc] = F(caller);
        caller.clear();
        lck.unlock();
    }
}
int main()
{

    std::string temp;
    uint num_external_inputs, num_outputs, num_gates;
    // std::cout << "Num external inputs: ";
    std::cin >> num_external_inputs;
    // std::cout << "Num outputs: ";
    std::cin >> num_outputs;
    // std::cout << "Input names (newline separated):\n";
    for (uint i = 0; i < num_external_inputs; i++)
    {
        std::cin >> temp;
        inputs.push_back(temp);
    }
    // std::cout << "Number of Gates: ";
    std::cin >> num_gates;
    // std::cout << "Gate Names (newline separated):\n";
    for (uint i = 0; i < num_gates; i++)
    {
        std::cin >> temp;
        gate_names.push_back(temp);
        name_to_idx[temp] = i;
    }
    // from external to gate
    // std::cout << "Input locations (Indices, not names, newline separated):\n";
    input_locs.resize(num_gates);
    for (uint i = 0; i < num_external_inputs + 1; i++)
    {
        std::getline(std::cin, temp);
        std::stringstream ss(temp);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        // list of ilocs
        std::vector<std::string> vstrings(begin, end);

        for (auto j : vstrings)
        {
            input_locs[name_to_idx[j]].push_back(-1 * (i));
        }
    }
    // std::cout << "Gate inputs (newline separated): ";
    for (uint i = 0; i < num_gates; i++)
    {
        std::getline(std::cin, temp);
        std::stringstream ss(temp);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        // list of ilocs
        std::vector<std::string> vstrings(begin, end);

        for (auto j : vstrings)
        {
            input_locs[i].push_back(name_to_idx[j]);
        }
    }
    // std::cout << "Gate types: ";
    for (uint i = 0; i < num_gates; i++)
    {
        std::cin >> temp;
        gate_types.push_back((uint)std::stoi(temp));
    }
    // std::cout << "Output Gate: ";
    std::cin >> output_gate;
    uint j = 0;
    output_bits.resize(num_gates, 0);
    outer_inputs.resize(num_external_inputs, 1);
    std::vector<std::thread> threads;
    // stop threads from really starting
    std::unique_lock<std::mutex> lck(m);
    for (auto i : input_locs)
    {
        threads.push_back(std::thread(&function_caller, functions[gate_types[j]], i, j));
        j++;
    }
    lck.unlock();
    // std::cout << std::endl;

    for (int j = 0; j < 20000; j++)
    {
        for (auto i : output_bits)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
    }

    uint sz = threads.size();
    ready = true;

    for (uint i = 0; i < sz; i++)
    {
        threads[i].join();
    }
    return 0;
}

/*
    // first get it working for hardcoded, then for input files

    // adjacency map of connections
    std::vector<bool> battery_inputs;

    class GATE
    {

    private:
        uint idx;
        uint num_inputs;
        uint num_outputs;
        std::vector<bool> inputs;
        std::vector<bool> outputs;
        std::vector<uint> idx_inputs;
        std::vector<uint> idx_outputs;
        std::vector<uint> battery_idx_inputs;

    public:
        uint logicType;
        void Gate(uint NI, uint NO, uint IDX, uint LT, std::vector<uint> II, std::vector<uint> IO,
                std::vector<uint> BII)
        {
            num_inputs = NI;
            num_outputs = NO;
            idx = IDX;
            logicType = LT;
            idx_inputs = II;
            idx_outputs = IO;
            battery_idx_inputs = BII;
        }
        void update_from_battery()
        {
            for (auto i : battery_idx_inputs)
            {
                battery_inputs
            }
        }
    };
    std::vector<GATE> gates;
*/
/*std::vector<std::string> gate_names;

std::vector<bool> external_inputs;
std::vector<std::thread> threads;
class GATE
{
  public:
    std::string name;
    uint index;
    uint type;
    bool curr_output;
    std::vector<GATE *> inputs;
    std::vector<uint> other_inputs;
    std::vector<std::string> outputs;
    GATE() {}
    GATE(std::string name, uint idx)
    {
        this->name = name;
        this->index = idx;
    }
    void set_output(std::vector<std::string> outputs) { this->outputs = outputs; }
    void print_output()
    {
        for (auto i : this->outputs)
            std::cout << i << " ";
        std::cout << std::endl;
    }
    void OUTPUT()
    {
        bool cOut = false;
        if (index == AND)
        {
            while (true)
            {
                cOut = inputs[0]->curr_output;
                for (auto i : inputs)
                {
                    cOut &= i->curr_output;
                }
                for (auto i : other_inputs)
                {
                    cOut &= external_inputs[i];
                }
                curr_output = cOut;

                delay;
            }
        }
        else if (index == OR)
        {
            while (true)
            {
                cOut = inputs[0]->curr_output;
                for (auto i : inputs)
                {
                    cOut |= i->curr_output;
                }
                for (auto i : other_inputs)
                {
                    cOut |= external_inputs[i];
                }
                curr_output = cOut;
                delay;
            }
        }
        else if (index == NOT)
        {
            while (true)
            {
                cOut = !inputs[0]->curr_output;
                curr_output = cOut;
                delay;
            }
        }
        else if (index == XOR)
        {
            while (true)
            {
                cOut = inputs[0]->curr_output;
                bool coutB = inputs[0]->curr_output;
                for (auto i : inputs)
                {
                    cOut &= i->curr_output;
                    coutB |= i->curr_output;
                }
                for (auto i : other_inputs)
                {
                    cOut &= external_inputs[i];
                    coutB |= external_inputs[i];
                }
                curr_output = !cOut && !coutB;
                delay;
            }
        }
        else if (index == NAND)
        {
            while (true)
            {
                cOut = inputs[0]->curr_output;
                for (auto i : inputs)
                {
                    cOut &= i->curr_output;
                }
                for (auto i : other_inputs)
                {
                    cOut &= external_inputs[i];
                }
                curr_output = !cOut;
                delay;
            }
        }
        else if (index == NOR)
        {
            while (true)
            {
                cOut = inputs[0]->curr_output;
                for (auto i : inputs)
                {
                    cOut |= i->curr_output;
                }
                for (auto i : other_inputs)
                {
                    cOut |= external_inputs[i];
                }
                curr_output = !cOut;
                delay;
            }
        }
    }
};
uint num_external_inputs;
std::string final_output;
std::unordered_map<std::string, GATE> map;

std::vector<std::vector<GATE *>> external_connections;
void randomized_inputs()
{
    while (true)
    {
        for (int i = 0; i < num_external_inputs; i++)
        {
            external_inputs[i] = rand() % 2;
        }

        delay;
    }
}
int main()
{
    // temporary string for reading in user input
    std::string temp_string;
    bool readNames, readOutputs, readTypes, readInputNum, readInputs;
    // read in user input
    while (std::getline(std::cin, temp_string))
    {
        if (temp_string[0] == '-' || temp_string[0] == '/')
            continue;
        if (!readNames)
        {
            readNames = 1;
            // break line into vector
            std::stringstream ss(temp_string);
            std::istream_iterator<std::string> begin(ss);
            std::istream_iterator<std::string> end;
            std::vector<std::string> vstrings(begin, end);
            gate_names = vstrings;
            uint idx = 0;
            for (auto name : gate_names)
            {
                // init the map
                map[name] = GATE(name, idx);
                idx++;
            }
        }
        else if (!readOutputs)
        {
            readOutputs = 1;
            for (auto i : gate_names)
            {
                std::stringstream ss(temp_string);
                std::istream_iterator<std::string> begin(ss);
                std::istream_iterator<std::string> end;
                // list of outputs.
                std::vector<std::string> vstrings(begin, end);
                map[i].set_output(vstrings);
                std::getline(std::cin, temp_string);
            }
        }
        else if (!readTypes)
        {

            uint temp;
            for (auto i : gate_names)
            {
                if (!readTypes)
                {
                    map[i].type = std::stoi(temp_string);
                    readTypes = 1;
                    continue;
                }
                std::cin >> temp;
                map[i].type = temp;
                std::getline(std::cin, temp_string);
            }
        }
        else if (!readInputNum)
        {
            readInputNum = 1;
            num_external_inputs = std::stoi(temp_string);
            external_inputs.resize(num_external_inputs, 0);
        }
        else if (!readInputs)
        {
            readInputs = 1;
            external_connections.resize(num_external_inputs);
            for (int j = 0; j < num_external_inputs; j++)
            {
                std::stringstream ss(temp_string);
                std::istream_iterator<std::string> begin(ss);
                std::istream_iterator<std::string> end;
                // list of outputs.
                std::vector<std::string> vstrings(begin, end);
                for (auto i : vstrings)
                {
                    external_connections[j].push_back(&map[i]);
                    map[i].other_inputs.push_back(j);
                }
                std::getline(std::cin, temp_string);
            }
        }
        else
        {
            std::cin >> final_output;
            break;
        }
    }
    // get the inputs situated
    for (auto i : gate_names)
    {
        for (auto j : map[i].outputs)
        {
            map[j].inputs.push_back(&map[i]);
        }
    }

    // testing with random inputs;

    std::thread RI(randomized_inputs);
    RI.detach();
    int j = 0;
    for (auto i : gate_names)
    {
        threads.push_back(std::thread(&GATE::OUTPUT, map[i]));
    }
    while (j < 999999999)
        j++;
    // for (uint i = 0; i < gate_names.size(); i++)
    //{
    //    threads[i].detach();
    //}
    for (int i = 0; i < 20; i++)
    {
        for (auto i : gate_names)
        {
            std::cout << map[i].curr_output << " ";
        }
        std::cout << "\n";
        delay;
    }

    return 0;
}
*/