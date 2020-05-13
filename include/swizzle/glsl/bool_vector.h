#pragma once

namespace swizzle
{
    namespace newww
    {

        template <typename BoolType, size_t... Indices>
        struct bool_vector;

        template <typename BoolType, size_t... Indices> bool_vector<BoolType, Indices...> bool_vector_type_builder(std::index_sequence<Indices...>);

        template <typename BoolType, size_t Size>
        using bvector = decltype(bool_vector_type_builder<BoolType>(std::make_index_sequence<Size>{}));

        
    }
}