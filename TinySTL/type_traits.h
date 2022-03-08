//
// Created by Cu1 on 2022/3/7.
//

#ifndef TINYSTL_TYPE_TRAITS_H
#define TINYSTL_TYPE_TRAITS_H

// 用于提取对象中的类型信息
#include <type_traits>

namespace tstl
{

    template <class T, T v>
        struct m_intergal_constant
    {
        static constexpr T value  = v;
    };

    template <bool b>
    using m_bool_constant = m_intergal_constant<bool, b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;

    /***********************************************************************************/
    //type traits

    // is_pair
    template <class T1, class T2>
        struct pair;

    template <class T>
            struct is_pair : tstl::m_false_type {};
    template <class T1, class T2>
            struct is_pair<tstl::pair<T1, T2>> : tstl::m_true_type {};
};

#endif //TINYSTL_TYPE_TRAITS_H
