#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__
#if defined(__MINGW32__)
#include <chrono>
#endif
#include <sstream>
#include <iostream>
#include <random>
#include <stack>
#include <functional>
#include <type_traits>

namespace seed {
    template <typename Engine>
    struct is_64bit
    {
        static const bool value = (sizeof(std::int64_t)
                                   == sizeof(typename Engine::result_type));
    };

#if defined(__MINGW32__)
    template <typename Engine>
    typename Engine::result_type Generate()
    {
        return static_cast<typename Engine::result_type>
            (std::chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count());
    }
#else
    template <typename Engine>
    typename std::enable_if<is_64bit<Engine>::value,
        typename Engine::result_type>::type
        Generate()
    {
        std::random_device rd;
        return ((unsigned long long)rd() << 32 | rd());
    }

    template <typename Engine>
    typename std::enable_if<!is_64bit<Engine>::value,
        typename Engine::result_type>::type
        Generate()
    {
        std::random_device rd;
        return rd();
    }
#endif
}

template <
    typename Engine,
    typename Dist,
    typename DataType
>
class RandomGen
{
public:
    RandomGen(typename Engine::result_type seed, DataType min, DataType max)
    {
        _init(seed, min, max);
    }

    RandomGen(DataType min, DataType max)
    {
        _init(seed::Generate<Engine>(), min, max);
    }
    DataType Generate()
    {
        m_counter++;
        return m_Random();
    }
    typename Engine::result_type Seed()
    {
        return m_seed;
    }

    void Seed(typename Engine::result_type seed)
    {
        _init(seed, m_min, m_max);;
    }

    void MinMax(DataType min, DataType max)
    {
        _init(m_seed, min, max);;
    }
    void Reset(typename Engine::result_type seed, DataType min, DataType max)
    {
        _init(seed, min, max);
    }
    void push_state()
    {
        state _state;
        _state.ss << m_Engine;
        _state.counter = m_counter;
        _state.seed = m_seed;
        _state.max = m_max;
        _state.min = m_min;
        m_States.push(std::move(_state));
    }
    void pop_state()
    {
        if (!(m_States.empty()))
        {
            m_seed = m_States.top().seed;
            m_Engine.seed(m_seed);

            m_States.top().ss >> m_Engine;

            m_counter = m_States.top().counter;
            m_Engine.discard(m_counter);

            m_max = m_States.top().max;
            m_min = m_States.top().min;
            _init(m_min, m_max, m_Engine);
            m_States.pop();
        }
    }
private:
    void _init(typename Engine::result_type seed, DataType min, DataType max)
    {
        m_max = max;
        m_min = min;
        m_seed = seed;
        m_counter = 0;
        m_Engine = std::move(Engine(seed));
        return _init(m_min, m_max, m_Engine);
    }

    void _init(DataType min, DataType max, Engine& engine)
    {
        m_Random = std::bind(Dist(min, max), engine);
    }
    Engine m_Engine;
    DataType m_min, m_max;
    typename Engine::result_type m_seed;
    std::function<DataType()> m_Random;
    unsigned long long m_counter;

    struct state
    {
        std::stringstream ss;
        unsigned long long counter;
        typename Engine::result_type seed;
        DataType min;
        DataType max;
    };
    std::stack<state> m_States;
};

template <typename DataType>
struct default_distribution_traits
{
    using type = typename std::conditional<
        std::is_floating_point<DataType>::value,
        std::uniform_real_distribution<DataType>,
        std::uniform_int_distribution<DataType>
    >::type;
};

template<typename DataType, typename Engine = std::mt19937_64>
using Random = RandomGen<Engine, typename default_distribution_traits<DataType>::type, DataType>;

// use this template for more control
template <
    typename Engine,
    template <typename> class Dist,
    typename DataType
>
using Random2 = RandomGen<Engine, Dist<DataType>, DataType>;

#endif //__RANDOM_GENERATOR_H__
