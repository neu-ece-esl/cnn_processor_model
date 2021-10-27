#include "pe.hh"

template <typename DataType>
PE<DataType>::PE(sc_module_name name, sc_trace_file* _tf) : sc_module(name), tf(_tf), psum_in("psum_in")
{
    this->resetWeightIdx();
    this->resetWeights();
    this->programmed = false;

    sc_trace(tf, this->psum_in, (this->psum_in.name()));
}

template <typename DataType>
void PE<DataType>::reset()
{
    this->resetWeightIdx();
    this->resetWeights();
    this->programmed = false;
}

template <typename DataType>
void PE<DataType>::resetWeightIdx()
{
    this->weight_idx = 0;
}

template <typename DataType>
void PE<DataType>::resetWeights()
{
    this->weights.clear();
}

template <typename DataType>
int PE<DataType>::currentWeight()
{
    return this->weights[this->weight_idx];
}


template <typename DataType>
void PE<DataType>::loadWeights(vector<int> &weights)
{
    this->weights = weights;
}

template <typename DataType>
void PE<DataType>::loadProgram(vector<Descriptor_2D> &_program)
{
    this->program.clear();
    for (auto &desc : _program)
    {
        this->program.push_back(desc);
    }
    this->prog_idx = 0;
    this->programmed = true;
}

template <typename DataType>
void PE<DataType>::updateState()
{
    if (this->programmed)
    {
        Descriptor_2D &current_desc = this->program.at(prog_idx);

        if (current_desc.state == DescriptorState::GENWAIT)
        {
            current_desc.x_counter--;
            if (current_desc.x_counter < 0)
            {
                current_desc.x_counter = current_desc.x_count;
                current_desc.y_counter--;
                weight_idx+=current_desc.y_modify;
            }
            if (current_desc.y_counter < 0)
            {
                this->prog_idx++;
            }
        }
        else if (current_desc.state == DescriptorState::WAIT)
        {
            current_desc.x_counter--;
            if (current_desc.x_counter < 0)
            {
                current_desc.x_counter = current_desc.x_count;
                current_desc.y_counter--;
            }
            if (current_desc.y_counter < 0)
            {
                this->prog_idx++;
            }
        }
        else if (current_desc.state == DescriptorState::SUSPENDED)
        {
            return;
        }
        else
        {
            throw std::exception("Invalid Descriptor in pe program");
        }
    }
    else
    {
        throw std::exception("Attempted to update pe state without it being programmed");
    }
}