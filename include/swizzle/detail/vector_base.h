// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    namespace detail
    {
        template <size_t Size, template <size_t, size_t, size_t, size_t> class TProxyGenerator, class TData>
        struct vector_base;

        template <template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyGenerator, class TData>
        struct vector_base<1, TProxyGenerator, TData>
        {
            union
            {
                TData m_data;

                typename TProxyGenerator<0>::type x, r, s;
                typename TProxyGenerator<0,0>::type xx, rr, ss;
                typename TProxyGenerator<0,0,0>::type xxx, rrr, sss;
                typename TProxyGenerator<0,0,0,0>::type xxxx, rrrr, ssss;
            };
        };

        template <template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyGenerator, class TData>
        struct vector_base<2, TProxyGenerator, TData>
        {
            union
            {
                TData m_data;

                struct
                {
                    typename TProxyGenerator<0>::type x;
                    typename TProxyGenerator<1>::type y;
                };

                struct
                {
                    typename TProxyGenerator<0>::type r;
                    typename TProxyGenerator<1>::type g;
                };

                struct
                {
                    typename TProxyGenerator<0>::type s;
                    typename TProxyGenerator<1>::type t;
                };

                typename TProxyGenerator<0,0>::type xx, rr, ss;
                typename TProxyGenerator<0,1>::type xy, rg, st;
                typename TProxyGenerator<1,0>::type yx, gr, ts;
                typename TProxyGenerator<1,1>::type yy, gg, tt;
                typename TProxyGenerator<0,0,0>::type xxx, rrr, sss;
                typename TProxyGenerator<0,0,1>::type xxy, rrg, sst;
                typename TProxyGenerator<0,1,0>::type xyx, rgr, sts;
                typename TProxyGenerator<0,1,1>::type xyy, rgg, stt;
                typename TProxyGenerator<1,0,0>::type yxx, grr, tss;
                typename TProxyGenerator<1,0,1>::type yxy, grg, tst;
                typename TProxyGenerator<1,1,0>::type yyx, ggr, tts;
                typename TProxyGenerator<1,1,1>::type yyy, ggg, ttt;
                typename TProxyGenerator<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyGenerator<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyGenerator<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyGenerator<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyGenerator<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyGenerator<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyGenerator<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyGenerator<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyGenerator<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyGenerator<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyGenerator<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyGenerator<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyGenerator<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyGenerator<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyGenerator<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyGenerator<1,1,1,1>::type yyyy, gggg, tttt;
            };
        };

        template <template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyGenerator, class TData>
        struct vector_base<3, TProxyGenerator, TData>
        {
            union
            {
                TData m_data;

                struct
                {
                    typename TProxyGenerator<0>::type x;
                    typename TProxyGenerator<1>::type y;
                    typename TProxyGenerator<2>::type z;
                };

                struct
                {
                    typename TProxyGenerator<0>::type r;
                    typename TProxyGenerator<1>::type g;
                    typename TProxyGenerator<2>::type b;
                };

                struct
                {
                    typename TProxyGenerator<0>::type s;
                    typename TProxyGenerator<1>::type t;
                    typename TProxyGenerator<2>::type p;
                };

                typename TProxyGenerator<0,0>::type xx, rr, ss;
                typename TProxyGenerator<0,1>::type xy, rg, st;
                typename TProxyGenerator<0,2>::type xz, rb, sp;
                typename TProxyGenerator<1,0>::type yx, gr, ts;
                typename TProxyGenerator<1,1>::type yy, gg, tt;
                typename TProxyGenerator<1,2>::type yz, gb, tp;
                typename TProxyGenerator<2,0>::type zx, br, ps;
                typename TProxyGenerator<2,1>::type zy, bg, pt;
                typename TProxyGenerator<2,2>::type zz, bb, pp;
                typename TProxyGenerator<0,0,0>::type xxx, rrr, sss;
                typename TProxyGenerator<0,0,1>::type xxy, rrg, sst;
                typename TProxyGenerator<0,0,2>::type xxz, rrb, ssp;
                typename TProxyGenerator<0,1,0>::type xyx, rgr, sts;
                typename TProxyGenerator<0,1,1>::type xyy, rgg, stt;
                typename TProxyGenerator<0,1,2>::type xyz, rgb, stp;
                typename TProxyGenerator<0,2,0>::type xzx, rbr, sps;
                typename TProxyGenerator<0,2,1>::type xzy, rbg, spt;
                typename TProxyGenerator<0,2,2>::type xzz, rbb, spp;
                typename TProxyGenerator<1,0,0>::type yxx, grr, tss;
                typename TProxyGenerator<1,0,1>::type yxy, grg, tst;
                typename TProxyGenerator<1,0,2>::type yxz, grb, tsp;
                typename TProxyGenerator<1,1,0>::type yyx, ggr, tts;
                typename TProxyGenerator<1,1,1>::type yyy, ggg, ttt;
                typename TProxyGenerator<1,1,2>::type yyz, ggb, ttp;
                typename TProxyGenerator<1,2,0>::type yzx, gbr, tps;
                typename TProxyGenerator<1,2,1>::type yzy, gbg, tpt;
                typename TProxyGenerator<1,2,2>::type yzz, gbb, tpp;
                typename TProxyGenerator<2,0,0>::type zxx, brr, pss;
                typename TProxyGenerator<2,0,1>::type zxy, brg, pst;
                typename TProxyGenerator<2,0,2>::type zxz, brb, psp;
                typename TProxyGenerator<2,1,0>::type zyx, bgr, pts;
                typename TProxyGenerator<2,1,1>::type zyy, bgg, ptt;
                typename TProxyGenerator<2,1,2>::type zyz, bgb, ptp;
                typename TProxyGenerator<2,2,0>::type zzx, bbr, pps;
                typename TProxyGenerator<2,2,1>::type zzy, bbg, ppt;
                typename TProxyGenerator<2,2,2>::type zzz, bbb, ppp;
                typename TProxyGenerator<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyGenerator<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyGenerator<0,0,0,2>::type xxxz, rrrb, sssp;
                typename TProxyGenerator<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyGenerator<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyGenerator<0,0,1,2>::type xxyz, rrgb, sstp;
                typename TProxyGenerator<0,0,2,0>::type xxzx, rrbr, ssps;
                typename TProxyGenerator<0,0,2,1>::type xxzy, rrbg, sspt;
                typename TProxyGenerator<0,0,2,2>::type xxzz, rrbb, sspp;
                typename TProxyGenerator<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyGenerator<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyGenerator<0,1,0,2>::type xyxz, rgrb, stsp;
                typename TProxyGenerator<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyGenerator<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyGenerator<0,1,1,2>::type xyyz, rggb, sttp;
                typename TProxyGenerator<0,1,2,0>::type xyzx, rgbr, stps;
                typename TProxyGenerator<0,1,2,1>::type xyzy, rgbg, stpt;
                typename TProxyGenerator<0,1,2,2>::type xyzz, rgbb, stpp;
                typename TProxyGenerator<0,2,0,0>::type xzxx, rbrr, spss;
                typename TProxyGenerator<0,2,0,1>::type xzxy, rbrg, spst;
                typename TProxyGenerator<0,2,0,2>::type xzxz, rbrb, spsp;
                typename TProxyGenerator<0,2,1,0>::type xzyx, rbgr, spts;
                typename TProxyGenerator<0,2,1,1>::type xzyy, rbgg, sptt;
                typename TProxyGenerator<0,2,1,2>::type xzyz, rbgb, sptp;
                typename TProxyGenerator<0,2,2,0>::type xzzx, rbbr, spps;
                typename TProxyGenerator<0,2,2,1>::type xzzy, rbbg, sppt;
                typename TProxyGenerator<0,2,2,2>::type xzzz, rbbb, sppp;
                typename TProxyGenerator<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyGenerator<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyGenerator<1,0,0,2>::type yxxz, grrb, tssp;
                typename TProxyGenerator<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyGenerator<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyGenerator<1,0,1,2>::type yxyz, grgb, tstp;
                typename TProxyGenerator<1,0,2,0>::type yxzx, grbr, tsps;
                typename TProxyGenerator<1,0,2,1>::type yxzy, grbg, tspt;
                typename TProxyGenerator<1,0,2,2>::type yxzz, grbb, tspp;
                typename TProxyGenerator<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyGenerator<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyGenerator<1,1,0,2>::type yyxz, ggrb, ttsp;
                typename TProxyGenerator<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyGenerator<1,1,1,1>::type yyyy, gggg, tttt;
                typename TProxyGenerator<1,1,1,2>::type yyyz, gggb, tttp;
                typename TProxyGenerator<1,1,2,0>::type yyzx, ggbr, ttps;
                typename TProxyGenerator<1,1,2,1>::type yyzy, ggbg, ttpt;
                typename TProxyGenerator<1,1,2,2>::type yyzz, ggbb, ttpp;
                typename TProxyGenerator<1,2,0,0>::type yzxx, gbrr, tpss;
                typename TProxyGenerator<1,2,0,1>::type yzxy, gbrg, tpst;
                typename TProxyGenerator<1,2,0,2>::type yzxz, gbrb, tpsp;
                typename TProxyGenerator<1,2,1,0>::type yzyx, gbgr, tpts;
                typename TProxyGenerator<1,2,1,1>::type yzyy, gbgg, tptt;
                typename TProxyGenerator<1,2,1,2>::type yzyz, gbgb, tptp;
                typename TProxyGenerator<1,2,2,0>::type yzzx, gbbr, tpps;
                typename TProxyGenerator<1,2,2,1>::type yzzy, gbbg, tppt;
                typename TProxyGenerator<1,2,2,2>::type yzzz, gbbb, tppp;
                typename TProxyGenerator<2,0,0,0>::type zxxx, brrr, psss;
                typename TProxyGenerator<2,0,0,1>::type zxxy, brrg, psst;
                typename TProxyGenerator<2,0,0,2>::type zxxz, brrb, pssp;
                typename TProxyGenerator<2,0,1,0>::type zxyx, brgr, psts;
                typename TProxyGenerator<2,0,1,1>::type zxyy, brgg, pstt;
                typename TProxyGenerator<2,0,1,2>::type zxyz, brgb, pstp;
                typename TProxyGenerator<2,0,2,0>::type zxzx, brbr, psps;
                typename TProxyGenerator<2,0,2,1>::type zxzy, brbg, pspt;
                typename TProxyGenerator<2,0,2,2>::type zxzz, brbb, pspp;
                typename TProxyGenerator<2,1,0,0>::type zyxx, bgrr, ptss;
                typename TProxyGenerator<2,1,0,1>::type zyxy, bgrg, ptst;
                typename TProxyGenerator<2,1,0,2>::type zyxz, bgrb, ptsp;
                typename TProxyGenerator<2,1,1,0>::type zyyx, bggr, ptts;
                typename TProxyGenerator<2,1,1,1>::type zyyy, bggg, pttt;
                typename TProxyGenerator<2,1,1,2>::type zyyz, bggb, pttp;
                typename TProxyGenerator<2,1,2,0>::type zyzx, bgbr, ptps;
                typename TProxyGenerator<2,1,2,1>::type zyzy, bgbg, ptpt;
                typename TProxyGenerator<2,1,2,2>::type zyzz, bgbb, ptpp;
                typename TProxyGenerator<2,2,0,0>::type zzxx, bbrr, ppss;
                typename TProxyGenerator<2,2,0,1>::type zzxy, bbrg, ppst;
                typename TProxyGenerator<2,2,0,2>::type zzxz, bbrb, ppsp;
                typename TProxyGenerator<2,2,1,0>::type zzyx, bbgr, ppts;
                typename TProxyGenerator<2,2,1,1>::type zzyy, bbgg, pptt;
                typename TProxyGenerator<2,2,1,2>::type zzyz, bbgb, pptp;
                typename TProxyGenerator<2,2,2,0>::type zzzx, bbbr, ppps;
                typename TProxyGenerator<2,2,2,1>::type zzzy, bbbg, pppt;
                typename TProxyGenerator<2,2,2,2>::type zzzz, bbbb, pppp;
            };
        };

        template <template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyGenerator, class TData>
        struct vector_base<4, TProxyGenerator, TData>
        {
            union
            {
                TData m_data;

                struct
                {
                    typename TProxyGenerator<0>::type x;
                    typename TProxyGenerator<1>::type y;
                    typename TProxyGenerator<2>::type z;
                    typename TProxyGenerator<3>::type w;
                };

                struct
                {
                    typename TProxyGenerator<0>::type r;
                    typename TProxyGenerator<1>::type g;
                    typename TProxyGenerator<2>::type b;
                    typename TProxyGenerator<3>::type a;
                };

                struct
                {
                    typename TProxyGenerator<0>::type s;
                    typename TProxyGenerator<1>::type t;
                    typename TProxyGenerator<2>::type p;
                    typename TProxyGenerator<3>::type q;
                };

                typename TProxyGenerator<0,0>::type xx, rr, ss;
                typename TProxyGenerator<0,1>::type xy, rg, st;
                typename TProxyGenerator<0,2>::type xz, rb, sp;
                typename TProxyGenerator<0,3>::type xw, ra, sq;
                typename TProxyGenerator<1,0>::type yx, gr, ts;
                typename TProxyGenerator<1,1>::type yy, gg, tt;
                typename TProxyGenerator<1,2>::type yz, gb, tp;
                typename TProxyGenerator<1,3>::type yw, ga, tq;
                typename TProxyGenerator<2,0>::type zx, br, ps;
                typename TProxyGenerator<2,1>::type zy, bg, pt;
                typename TProxyGenerator<2,2>::type zz, bb, pp;
                typename TProxyGenerator<2,3>::type zw, ba, pq;
                typename TProxyGenerator<3,0>::type wx, ar, qs;
                typename TProxyGenerator<3,1>::type wy, ag, qt;
                typename TProxyGenerator<3,2>::type wz, ab, qp;
                typename TProxyGenerator<3,3>::type ww, aa, qq;
                typename TProxyGenerator<0,0,0>::type xxx, rrr, sss;
                typename TProxyGenerator<0,0,1>::type xxy, rrg, sst;
                typename TProxyGenerator<0,0,2>::type xxz, rrb, ssp;
                typename TProxyGenerator<0,0,3>::type xxw, rra, ssq;
                typename TProxyGenerator<0,1,0>::type xyx, rgr, sts;
                typename TProxyGenerator<0,1,1>::type xyy, rgg, stt;
                typename TProxyGenerator<0,1,2>::type xyz, rgb, stp;
                typename TProxyGenerator<0,1,3>::type xyw, rga, stq;
                typename TProxyGenerator<0,2,0>::type xzx, rbr, sps;
                typename TProxyGenerator<0,2,1>::type xzy, rbg, spt;
                typename TProxyGenerator<0,2,2>::type xzz, rbb, spp;
                typename TProxyGenerator<0,2,3>::type xzw, rba, spq;
                typename TProxyGenerator<0,3,0>::type xwx, rar, sqs;
                typename TProxyGenerator<0,3,1>::type xwy, rag, sqt;
                typename TProxyGenerator<0,3,2>::type xwz, rab, sqp;
                typename TProxyGenerator<0,3,3>::type xww, raa, sqq;
                typename TProxyGenerator<1,0,0>::type yxx, grr, tss;
                typename TProxyGenerator<1,0,1>::type yxy, grg, tst;
                typename TProxyGenerator<1,0,2>::type yxz, grb, tsp;
                typename TProxyGenerator<1,0,3>::type yxw, gra, tsq;
                typename TProxyGenerator<1,1,0>::type yyx, ggr, tts;
                typename TProxyGenerator<1,1,1>::type yyy, ggg, ttt;
                typename TProxyGenerator<1,1,2>::type yyz, ggb, ttp;
                typename TProxyGenerator<1,1,3>::type yyw, gga, ttq;
                typename TProxyGenerator<1,2,0>::type yzx, gbr, tps;
                typename TProxyGenerator<1,2,1>::type yzy, gbg, tpt;
                typename TProxyGenerator<1,2,2>::type yzz, gbb, tpp;
                typename TProxyGenerator<1,2,3>::type yzw, gba, tpq;
                typename TProxyGenerator<1,3,0>::type ywx, gar, tqs;
                typename TProxyGenerator<1,3,1>::type ywy, gag, tqt;
                typename TProxyGenerator<1,3,2>::type ywz, gab, tqp;
                typename TProxyGenerator<1,3,3>::type yww, gaa, tqq;
                typename TProxyGenerator<2,0,0>::type zxx, brr, pss;
                typename TProxyGenerator<2,0,1>::type zxy, brg, pst;
                typename TProxyGenerator<2,0,2>::type zxz, brb, psp;
                typename TProxyGenerator<2,0,3>::type zxw, bra, psq;
                typename TProxyGenerator<2,1,0>::type zyx, bgr, pts;
                typename TProxyGenerator<2,1,1>::type zyy, bgg, ptt;
                typename TProxyGenerator<2,1,2>::type zyz, bgb, ptp;
                typename TProxyGenerator<2,1,3>::type zyw, bga, ptq;
                typename TProxyGenerator<2,2,0>::type zzx, bbr, pps;
                typename TProxyGenerator<2,2,1>::type zzy, bbg, ppt;
                typename TProxyGenerator<2,2,2>::type zzz, bbb, ppp;
                typename TProxyGenerator<2,2,3>::type zzw, bba, ppq;
                typename TProxyGenerator<2,3,0>::type zwx, bar, pqs;
                typename TProxyGenerator<2,3,1>::type zwy, bag, pqt;
                typename TProxyGenerator<2,3,2>::type zwz, bab, pqp;
                typename TProxyGenerator<2,3,3>::type zww, baa, pqq;
                typename TProxyGenerator<3,0,0>::type wxx, arr, qss;
                typename TProxyGenerator<3,0,1>::type wxy, arg, qst;
                typename TProxyGenerator<3,0,2>::type wxz, arb, qsp;
                typename TProxyGenerator<3,0,3>::type wxw, ara, qsq;
                typename TProxyGenerator<3,1,0>::type wyx, agr, qts;
                typename TProxyGenerator<3,1,1>::type wyy, agg, qtt;
                typename TProxyGenerator<3,1,2>::type wyz, agb, qtp;
                typename TProxyGenerator<3,1,3>::type wyw, aga, qtq;
                typename TProxyGenerator<3,2,0>::type wzx, abr, qps;
                typename TProxyGenerator<3,2,1>::type wzy, abg, qpt;
                typename TProxyGenerator<3,2,2>::type wzz, abb, qpp;
                typename TProxyGenerator<3,2,3>::type wzw, aba, qpq;
                typename TProxyGenerator<3,3,0>::type wwx, aar, qqs;
                typename TProxyGenerator<3,3,1>::type wwy, aag, qqt;
                typename TProxyGenerator<3,3,2>::type wwz, aab, qqp;
                typename TProxyGenerator<3,3,3>::type www, aaa, qqq;
                typename TProxyGenerator<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyGenerator<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyGenerator<0,0,0,2>::type xxxz, rrrb, sssp;
                typename TProxyGenerator<0,0,0,3>::type xxxw, rrra, sssq;
                typename TProxyGenerator<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyGenerator<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyGenerator<0,0,1,2>::type xxyz, rrgb, sstp;
                typename TProxyGenerator<0,0,1,3>::type xxyw, rrga, sstq;
                typename TProxyGenerator<0,0,2,0>::type xxzx, rrbr, ssps;
                typename TProxyGenerator<0,0,2,1>::type xxzy, rrbg, sspt;
                typename TProxyGenerator<0,0,2,2>::type xxzz, rrbb, sspp;
                typename TProxyGenerator<0,0,2,3>::type xxzw, rrba, sspq;
                typename TProxyGenerator<0,0,3,0>::type xxwx, rrar, ssqs;
                typename TProxyGenerator<0,0,3,1>::type xxwy, rrag, ssqt;
                typename TProxyGenerator<0,0,3,2>::type xxwz, rrab, ssqp;
                typename TProxyGenerator<0,0,3,3>::type xxww, rraa, ssqq;
                typename TProxyGenerator<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyGenerator<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyGenerator<0,1,0,2>::type xyxz, rgrb, stsp;
                typename TProxyGenerator<0,1,0,3>::type xyxw, rgra, stsq;
                typename TProxyGenerator<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyGenerator<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyGenerator<0,1,1,2>::type xyyz, rggb, sttp;
                typename TProxyGenerator<0,1,1,3>::type xyyw, rgga, sttq;
                typename TProxyGenerator<0,1,2,0>::type xyzx, rgbr, stps;
                typename TProxyGenerator<0,1,2,1>::type xyzy, rgbg, stpt;
                typename TProxyGenerator<0,1,2,2>::type xyzz, rgbb, stpp;
                typename TProxyGenerator<0,1,2,3>::type xyzw, rgba, stpq;
                typename TProxyGenerator<0,1,3,0>::type xywx, rgar, stqs;
                typename TProxyGenerator<0,1,3,1>::type xywy, rgag, stqt;
                typename TProxyGenerator<0,1,3,2>::type xywz, rgab, stqp;
                typename TProxyGenerator<0,1,3,3>::type xyww, rgaa, stqq;
                typename TProxyGenerator<0,2,0,0>::type xzxx, rbrr, spss;
                typename TProxyGenerator<0,2,0,1>::type xzxy, rbrg, spst;
                typename TProxyGenerator<0,2,0,2>::type xzxz, rbrb, spsp;
                typename TProxyGenerator<0,2,0,3>::type xzxw, rbra, spsq;
                typename TProxyGenerator<0,2,1,0>::type xzyx, rbgr, spts;
                typename TProxyGenerator<0,2,1,1>::type xzyy, rbgg, sptt;
                typename TProxyGenerator<0,2,1,2>::type xzyz, rbgb, sptp;
                typename TProxyGenerator<0,2,1,3>::type xzyw, rbga, sptq;
                typename TProxyGenerator<0,2,2,0>::type xzzx, rbbr, spps;
                typename TProxyGenerator<0,2,2,1>::type xzzy, rbbg, sppt;
                typename TProxyGenerator<0,2,2,2>::type xzzz, rbbb, sppp;
                typename TProxyGenerator<0,2,2,3>::type xzzw, rbba, sppq;
                typename TProxyGenerator<0,2,3,0>::type xzwx, rbar, spqs;
                typename TProxyGenerator<0,2,3,1>::type xzwy, rbag, spqt;
                typename TProxyGenerator<0,2,3,2>::type xzwz, rbab, spqp;
                typename TProxyGenerator<0,2,3,3>::type xzww, rbaa, spqq;
                typename TProxyGenerator<0,3,0,0>::type xwxx, rarr, sqss;
                typename TProxyGenerator<0,3,0,1>::type xwxy, rarg, sqst;
                typename TProxyGenerator<0,3,0,2>::type xwxz, rarb, sqsp;
                typename TProxyGenerator<0,3,0,3>::type xwxw, rara, sqsq;
                typename TProxyGenerator<0,3,1,0>::type xwyx, ragr, sqts;
                typename TProxyGenerator<0,3,1,1>::type xwyy, ragg, sqtt;
                typename TProxyGenerator<0,3,1,2>::type xwyz, ragb, sqtp;
                typename TProxyGenerator<0,3,1,3>::type xwyw, raga, sqtq;
                typename TProxyGenerator<0,3,2,0>::type xwzx, rabr, sqps;
                typename TProxyGenerator<0,3,2,1>::type xwzy, rabg, sqpt;
                typename TProxyGenerator<0,3,2,2>::type xwzz, rabb, sqpp;
                typename TProxyGenerator<0,3,2,3>::type xwzw, raba, sqpq;
                typename TProxyGenerator<0,3,3,0>::type xwwx, raar, sqqs;
                typename TProxyGenerator<0,3,3,1>::type xwwy, raag, sqqt;
                typename TProxyGenerator<0,3,3,2>::type xwwz, raab, sqqp;
                typename TProxyGenerator<0,3,3,3>::type xwww, raaa, sqqq;
                typename TProxyGenerator<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyGenerator<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyGenerator<1,0,0,2>::type yxxz, grrb, tssp;
                typename TProxyGenerator<1,0,0,3>::type yxxw, grra, tssq;
                typename TProxyGenerator<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyGenerator<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyGenerator<1,0,1,2>::type yxyz, grgb, tstp;
                typename TProxyGenerator<1,0,1,3>::type yxyw, grga, tstq;
                typename TProxyGenerator<1,0,2,0>::type yxzx, grbr, tsps;
                typename TProxyGenerator<1,0,2,1>::type yxzy, grbg, tspt;
                typename TProxyGenerator<1,0,2,2>::type yxzz, grbb, tspp;
                typename TProxyGenerator<1,0,2,3>::type yxzw, grba, tspq;
                typename TProxyGenerator<1,0,3,0>::type yxwx, grar, tsqs;
                typename TProxyGenerator<1,0,3,1>::type yxwy, grag, tsqt;
                typename TProxyGenerator<1,0,3,2>::type yxwz, grab, tsqp;
                typename TProxyGenerator<1,0,3,3>::type yxww, graa, tsqq;
                typename TProxyGenerator<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyGenerator<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyGenerator<1,1,0,2>::type yyxz, ggrb, ttsp;
                typename TProxyGenerator<1,1,0,3>::type yyxw, ggra, ttsq;
                typename TProxyGenerator<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyGenerator<1,1,1,1>::type yyyy, gggg, tttt;
                typename TProxyGenerator<1,1,1,2>::type yyyz, gggb, tttp;
                typename TProxyGenerator<1,1,1,3>::type yyyw, ggga, tttq;
                typename TProxyGenerator<1,1,2,0>::type yyzx, ggbr, ttps;
                typename TProxyGenerator<1,1,2,1>::type yyzy, ggbg, ttpt;
                typename TProxyGenerator<1,1,2,2>::type yyzz, ggbb, ttpp;
                typename TProxyGenerator<1,1,2,3>::type yyzw, ggba, ttpq;
                typename TProxyGenerator<1,1,3,0>::type yywx, ggar, ttqs;
                typename TProxyGenerator<1,1,3,1>::type yywy, ggag, ttqt;
                typename TProxyGenerator<1,1,3,2>::type yywz, ggab, ttqp;
                typename TProxyGenerator<1,1,3,3>::type yyww, ggaa, ttqq;
                typename TProxyGenerator<1,2,0,0>::type yzxx, gbrr, tpss;
                typename TProxyGenerator<1,2,0,1>::type yzxy, gbrg, tpst;
                typename TProxyGenerator<1,2,0,2>::type yzxz, gbrb, tpsp;
                typename TProxyGenerator<1,2,0,3>::type yzxw, gbra, tpsq;
                typename TProxyGenerator<1,2,1,0>::type yzyx, gbgr, tpts;
                typename TProxyGenerator<1,2,1,1>::type yzyy, gbgg, tptt;
                typename TProxyGenerator<1,2,1,2>::type yzyz, gbgb, tptp;
                typename TProxyGenerator<1,2,1,3>::type yzyw, gbga, tptq;
                typename TProxyGenerator<1,2,2,0>::type yzzx, gbbr, tpps;
                typename TProxyGenerator<1,2,2,1>::type yzzy, gbbg, tppt;
                typename TProxyGenerator<1,2,2,2>::type yzzz, gbbb, tppp;
                typename TProxyGenerator<1,2,2,3>::type yzzw, gbba, tppq;
                typename TProxyGenerator<1,2,3,0>::type yzwx, gbar, tpqs;
                typename TProxyGenerator<1,2,3,1>::type yzwy, gbag, tpqt;
                typename TProxyGenerator<1,2,3,2>::type yzwz, gbab, tpqp;
                typename TProxyGenerator<1,2,3,3>::type yzww, gbaa, tpqq;
                typename TProxyGenerator<1,3,0,0>::type ywxx, garr, tqss;
                typename TProxyGenerator<1,3,0,1>::type ywxy, garg, tqst;
                typename TProxyGenerator<1,3,0,2>::type ywxz, garb, tqsp;
                typename TProxyGenerator<1,3,0,3>::type ywxw, gara, tqsq;
                typename TProxyGenerator<1,3,1,0>::type ywyx, gagr, tqts;
                typename TProxyGenerator<1,3,1,1>::type ywyy, gagg, tqtt;
                typename TProxyGenerator<1,3,1,2>::type ywyz, gagb, tqtp;
                typename TProxyGenerator<1,3,1,3>::type ywyw, gaga, tqtq;
                typename TProxyGenerator<1,3,2,0>::type ywzx, gabr, tqps;
                typename TProxyGenerator<1,3,2,1>::type ywzy, gabg, tqpt;
                typename TProxyGenerator<1,3,2,2>::type ywzz, gabb, tqpp;
                typename TProxyGenerator<1,3,2,3>::type ywzw, gaba, tqpq;
                typename TProxyGenerator<1,3,3,0>::type ywwx, gaar, tqqs;
                typename TProxyGenerator<1,3,3,1>::type ywwy, gaag, tqqt;
                typename TProxyGenerator<1,3,3,2>::type ywwz, gaab, tqqp;
                typename TProxyGenerator<1,3,3,3>::type ywww, gaaa, tqqq;
                typename TProxyGenerator<2,0,0,0>::type zxxx, brrr, psss;
                typename TProxyGenerator<2,0,0,1>::type zxxy, brrg, psst;
                typename TProxyGenerator<2,0,0,2>::type zxxz, brrb, pssp;
                typename TProxyGenerator<2,0,0,3>::type zxxw, brra, pssq;
                typename TProxyGenerator<2,0,1,0>::type zxyx, brgr, psts;
                typename TProxyGenerator<2,0,1,1>::type zxyy, brgg, pstt;
                typename TProxyGenerator<2,0,1,2>::type zxyz, brgb, pstp;
                typename TProxyGenerator<2,0,1,3>::type zxyw, brga, pstq;
                typename TProxyGenerator<2,0,2,0>::type zxzx, brbr, psps;
                typename TProxyGenerator<2,0,2,1>::type zxzy, brbg, pspt;
                typename TProxyGenerator<2,0,2,2>::type zxzz, brbb, pspp;
                typename TProxyGenerator<2,0,2,3>::type zxzw, brba, pspq;
                typename TProxyGenerator<2,0,3,0>::type zxwx, brar, psqs;
                typename TProxyGenerator<2,0,3,1>::type zxwy, brag, psqt;
                typename TProxyGenerator<2,0,3,2>::type zxwz, brab, psqp;
                typename TProxyGenerator<2,0,3,3>::type zxww, braa, psqq;
                typename TProxyGenerator<2,1,0,0>::type zyxx, bgrr, ptss;
                typename TProxyGenerator<2,1,0,1>::type zyxy, bgrg, ptst;
                typename TProxyGenerator<2,1,0,2>::type zyxz, bgrb, ptsp;
                typename TProxyGenerator<2,1,0,3>::type zyxw, bgra, ptsq;
                typename TProxyGenerator<2,1,1,0>::type zyyx, bggr, ptts;
                typename TProxyGenerator<2,1,1,1>::type zyyy, bggg, pttt;
                typename TProxyGenerator<2,1,1,2>::type zyyz, bggb, pttp;
                typename TProxyGenerator<2,1,1,3>::type zyyw, bgga, pttq;
                typename TProxyGenerator<2,1,2,0>::type zyzx, bgbr, ptps;
                typename TProxyGenerator<2,1,2,1>::type zyzy, bgbg, ptpt;
                typename TProxyGenerator<2,1,2,2>::type zyzz, bgbb, ptpp;
                typename TProxyGenerator<2,1,2,3>::type zyzw, bgba, ptpq;
                typename TProxyGenerator<2,1,3,0>::type zywx, bgar, ptqs;
                typename TProxyGenerator<2,1,3,1>::type zywy, bgag, ptqt;
                typename TProxyGenerator<2,1,3,2>::type zywz, bgab, ptqp;
                typename TProxyGenerator<2,1,3,3>::type zyww, bgaa, ptqq;
                typename TProxyGenerator<2,2,0,0>::type zzxx, bbrr, ppss;
                typename TProxyGenerator<2,2,0,1>::type zzxy, bbrg, ppst;
                typename TProxyGenerator<2,2,0,2>::type zzxz, bbrb, ppsp;
                typename TProxyGenerator<2,2,0,3>::type zzxw, bbra, ppsq;
                typename TProxyGenerator<2,2,1,0>::type zzyx, bbgr, ppts;
                typename TProxyGenerator<2,2,1,1>::type zzyy, bbgg, pptt;
                typename TProxyGenerator<2,2,1,2>::type zzyz, bbgb, pptp;
                typename TProxyGenerator<2,2,1,3>::type zzyw, bbga, pptq;
                typename TProxyGenerator<2,2,2,0>::type zzzx, bbbr, ppps;
                typename TProxyGenerator<2,2,2,1>::type zzzy, bbbg, pppt;
                typename TProxyGenerator<2,2,2,2>::type zzzz, bbbb, pppp;
                typename TProxyGenerator<2,2,2,3>::type zzzw, bbba, pppq;
                typename TProxyGenerator<2,2,3,0>::type zzwx, bbar, ppqs;
                typename TProxyGenerator<2,2,3,1>::type zzwy, bbag, ppqt;
                typename TProxyGenerator<2,2,3,2>::type zzwz, bbab, ppqp;
                typename TProxyGenerator<2,2,3,3>::type zzww, bbaa, ppqq;
                typename TProxyGenerator<2,3,0,0>::type zwxx, barr, pqss;
                typename TProxyGenerator<2,3,0,1>::type zwxy, barg, pqst;
                typename TProxyGenerator<2,3,0,2>::type zwxz, barb, pqsp;
                typename TProxyGenerator<2,3,0,3>::type zwxw, bara, pqsq;
                typename TProxyGenerator<2,3,1,0>::type zwyx, bagr, pqts;
                typename TProxyGenerator<2,3,1,1>::type zwyy, bagg, pqtt;
                typename TProxyGenerator<2,3,1,2>::type zwyz, bagb, pqtp;
                typename TProxyGenerator<2,3,1,3>::type zwyw, baga, pqtq;
                typename TProxyGenerator<2,3,2,0>::type zwzx, babr, pqps;
                typename TProxyGenerator<2,3,2,1>::type zwzy, babg, pqpt;
                typename TProxyGenerator<2,3,2,2>::type zwzz, babb, pqpp;
                typename TProxyGenerator<2,3,2,3>::type zwzw, baba, pqpq;
                typename TProxyGenerator<2,3,3,0>::type zwwx, baar, pqqs;
                typename TProxyGenerator<2,3,3,1>::type zwwy, baag, pqqt;
                typename TProxyGenerator<2,3,3,2>::type zwwz, baab, pqqp;
                typename TProxyGenerator<2,3,3,3>::type zwww, baaa, pqqq;
                typename TProxyGenerator<3,0,0,0>::type wxxx, arrr, qsss;
                typename TProxyGenerator<3,0,0,1>::type wxxy, arrg, qsst;
                typename TProxyGenerator<3,0,0,2>::type wxxz, arrb, qssp;
                typename TProxyGenerator<3,0,0,3>::type wxxw, arra, qssq;
                typename TProxyGenerator<3,0,1,0>::type wxyx, argr, qsts;
                typename TProxyGenerator<3,0,1,1>::type wxyy, argg, qstt;
                typename TProxyGenerator<3,0,1,2>::type wxyz, argb, qstp;
                typename TProxyGenerator<3,0,1,3>::type wxyw, arga, qstq;
                typename TProxyGenerator<3,0,2,0>::type wxzx, arbr, qsps;
                typename TProxyGenerator<3,0,2,1>::type wxzy, arbg, qspt;
                typename TProxyGenerator<3,0,2,2>::type wxzz, arbb, qspp;
                typename TProxyGenerator<3,0,2,3>::type wxzw, arba, qspq;
                typename TProxyGenerator<3,0,3,0>::type wxwx, arar, qsqs;
                typename TProxyGenerator<3,0,3,1>::type wxwy, arag, qsqt;
                typename TProxyGenerator<3,0,3,2>::type wxwz, arab, qsqp;
                typename TProxyGenerator<3,0,3,3>::type wxww, araa, qsqq;
                typename TProxyGenerator<3,1,0,0>::type wyxx, agrr, qtss;
                typename TProxyGenerator<3,1,0,1>::type wyxy, agrg, qtst;
                typename TProxyGenerator<3,1,0,2>::type wyxz, agrb, qtsp;
                typename TProxyGenerator<3,1,0,3>::type wyxw, agra, qtsq;
                typename TProxyGenerator<3,1,1,0>::type wyyx, aggr, qtts;
                typename TProxyGenerator<3,1,1,1>::type wyyy, aggg, qttt;
                typename TProxyGenerator<3,1,1,2>::type wyyz, aggb, qttp;
                typename TProxyGenerator<3,1,1,3>::type wyyw, agga, qttq;
                typename TProxyGenerator<3,1,2,0>::type wyzx, agbr, qtps;
                typename TProxyGenerator<3,1,2,1>::type wyzy, agbg, qtpt;
                typename TProxyGenerator<3,1,2,2>::type wyzz, agbb, qtpp;
                typename TProxyGenerator<3,1,2,3>::type wyzw, agba, qtpq;
                typename TProxyGenerator<3,1,3,0>::type wywx, agar, qtqs;
                typename TProxyGenerator<3,1,3,1>::type wywy, agag, qtqt;
                typename TProxyGenerator<3,1,3,2>::type wywz, agab, qtqp;
                typename TProxyGenerator<3,1,3,3>::type wyww, agaa, qtqq;
                typename TProxyGenerator<3,2,0,0>::type wzxx, abrr, qpss;
                typename TProxyGenerator<3,2,0,1>::type wzxy, abrg, qpst;
                typename TProxyGenerator<3,2,0,2>::type wzxz, abrb, qpsp;
                typename TProxyGenerator<3,2,0,3>::type wzxw, abra, qpsq;
                typename TProxyGenerator<3,2,1,0>::type wzyx, abgr, qpts;
                typename TProxyGenerator<3,2,1,1>::type wzyy, abgg, qptt;
                typename TProxyGenerator<3,2,1,2>::type wzyz, abgb, qptp;
                typename TProxyGenerator<3,2,1,3>::type wzyw, abga, qptq;
                typename TProxyGenerator<3,2,2,0>::type wzzx, abbr, qpps;
                typename TProxyGenerator<3,2,2,1>::type wzzy, abbg, qppt;
                typename TProxyGenerator<3,2,2,2>::type wzzz, abbb, qppp;
                typename TProxyGenerator<3,2,2,3>::type wzzw, abba, qppq;
                typename TProxyGenerator<3,2,3,0>::type wzwx, abar, qpqs;
                typename TProxyGenerator<3,2,3,1>::type wzwy, abag, qpqt;
                typename TProxyGenerator<3,2,3,2>::type wzwz, abab, qpqp;
                typename TProxyGenerator<3,2,3,3>::type wzww, abaa, qpqq;
                typename TProxyGenerator<3,3,0,0>::type wwxx, aarr, qqss;
                typename TProxyGenerator<3,3,0,1>::type wwxy, aarg, qqst;
                typename TProxyGenerator<3,3,0,2>::type wwxz, aarb, qqsp;
                typename TProxyGenerator<3,3,0,3>::type wwxw, aara, qqsq;
                typename TProxyGenerator<3,3,1,0>::type wwyx, aagr, qqts;
                typename TProxyGenerator<3,3,1,1>::type wwyy, aagg, qqtt;
                typename TProxyGenerator<3,3,1,2>::type wwyz, aagb, qqtp;
                typename TProxyGenerator<3,3,1,3>::type wwyw, aaga, qqtq;
                typename TProxyGenerator<3,3,2,0>::type wwzx, aabr, qqps;
                typename TProxyGenerator<3,3,2,1>::type wwzy, aabg, qqpt;
                typename TProxyGenerator<3,3,2,2>::type wwzz, aabb, qqpp;
                typename TProxyGenerator<3,3,2,3>::type wwzw, aaba, qqpq;
                typename TProxyGenerator<3,3,3,0>::type wwwx, aaar, qqqs;
                typename TProxyGenerator<3,3,3,1>::type wwwy, aaag, qqqt;
                typename TProxyGenerator<3,3,3,2>::type wwwz, aaab, qqqp;
                typename TProxyGenerator<3,3,3,3>::type wwww, aaaa, qqqq;
            };
        };
    }
}