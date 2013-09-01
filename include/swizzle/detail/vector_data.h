#pragma once

#include <array>

namespace swizzle
{
    namespace detail
    {
        template <class TScalar, size_t Size, template <size_t, size_t, size_t, size_t> class TProxyFactory, class TData=std::array<TScalar,Size> >
        struct vector_data;

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 1, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 1;

            union
            {
                TData m_data;

                /*scalar_type x;*/

                typename TProxyFactory<0>::type x, r, s;
                typename TProxyFactory<0,0>::type xx, rr, ss;
                typename TProxyFactory<0,0,0>::type xxx, rrr, sss;
                typename TProxyFactory<0,0,0,0>::type xxxx, rrrr, ssss;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 2, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 2;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                };*/

                typename TProxyFactory<0>::type x, r, s;
                typename TProxyFactory<1>::type y, g, t;
                typename TProxyFactory<0,0>::type xx, rr, ss;
                typename TProxyFactory<0,1>::type xy, rg, st;
                typename TProxyFactory<1,0>::type yx, gr, ts;
                typename TProxyFactory<1,1>::type yy, gg, tt;
                typename TProxyFactory<0,0,0>::type xxx, rrr, sss;
                typename TProxyFactory<0,0,1>::type xxy, rrg, sst;
                typename TProxyFactory<0,1,0>::type xyx, rgr, sts;
                typename TProxyFactory<0,1,1>::type xyy, rgg, stt;
                typename TProxyFactory<1,0,0>::type yxx, grr, tss;
                typename TProxyFactory<1,0,1>::type yxy, grg, tst;
                typename TProxyFactory<1,1,0>::type yyx, ggr, tts;
                typename TProxyFactory<1,1,1>::type yyy, ggg, ttt;
                typename TProxyFactory<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyFactory<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyFactory<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyFactory<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyFactory<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyFactory<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyFactory<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyFactory<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyFactory<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyFactory<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyFactory<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyFactory<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyFactory<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyFactory<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyFactory<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyFactory<1,1,1,1>::type yyyy, gggg, tttt;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 3, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 3;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                };*/

                typename TProxyFactory<0>::type x, r, s;
                typename TProxyFactory<1>::type y, g, t;
                typename TProxyFactory<2>::type z, b, p;
                typename TProxyFactory<0,0>::type xx, rr, ss;
                typename TProxyFactory<0,1>::type xy, rg, st;
                typename TProxyFactory<0,2>::type xz, rb, sp;
                typename TProxyFactory<1,0>::type yx, gr, ts;
                typename TProxyFactory<1,1>::type yy, gg, tt;
                typename TProxyFactory<1,2>::type yz, gb, tp;
                typename TProxyFactory<2,0>::type zx, br, ps;
                typename TProxyFactory<2,1>::type zy, bg, pt;
                typename TProxyFactory<2,2>::type zz, bb, pp;
                typename TProxyFactory<0,0,0>::type xxx, rrr, sss;
                typename TProxyFactory<0,0,1>::type xxy, rrg, sst;
                typename TProxyFactory<0,0,2>::type xxz, rrb, ssp;
                typename TProxyFactory<0,1,0>::type xyx, rgr, sts;
                typename TProxyFactory<0,1,1>::type xyy, rgg, stt;
                typename TProxyFactory<0,1,2>::type xyz, rgb, stp;
                typename TProxyFactory<0,2,0>::type xzx, rbr, sps;
                typename TProxyFactory<0,2,1>::type xzy, rbg, spt;
                typename TProxyFactory<0,2,2>::type xzz, rbb, spp;
                typename TProxyFactory<1,0,0>::type yxx, grr, tss;
                typename TProxyFactory<1,0,1>::type yxy, grg, tst;
                typename TProxyFactory<1,0,2>::type yxz, grb, tsp;
                typename TProxyFactory<1,1,0>::type yyx, ggr, tts;
                typename TProxyFactory<1,1,1>::type yyy, ggg, ttt;
                typename TProxyFactory<1,1,2>::type yyz, ggb, ttp;
                typename TProxyFactory<1,2,0>::type yzx, gbr, tps;
                typename TProxyFactory<1,2,1>::type yzy, gbg, tpt;
                typename TProxyFactory<1,2,2>::type yzz, gbb, tpp;
                typename TProxyFactory<2,0,0>::type zxx, brr, pss;
                typename TProxyFactory<2,0,1>::type zxy, brg, pst;
                typename TProxyFactory<2,0,2>::type zxz, brb, psp;
                typename TProxyFactory<2,1,0>::type zyx, bgr, pts;
                typename TProxyFactory<2,1,1>::type zyy, bgg, ptt;
                typename TProxyFactory<2,1,2>::type zyz, bgb, ptp;
                typename TProxyFactory<2,2,0>::type zzx, bbr, pps;
                typename TProxyFactory<2,2,1>::type zzy, bbg, ppt;
                typename TProxyFactory<2,2,2>::type zzz, bbb, ppp;
                typename TProxyFactory<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyFactory<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyFactory<0,0,0,2>::type xxxz, rrrb, sssp;
                typename TProxyFactory<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyFactory<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyFactory<0,0,1,2>::type xxyz, rrgb, sstp;
                typename TProxyFactory<0,0,2,0>::type xxzx, rrbr, ssps;
                typename TProxyFactory<0,0,2,1>::type xxzy, rrbg, sspt;
                typename TProxyFactory<0,0,2,2>::type xxzz, rrbb, sspp;
                typename TProxyFactory<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyFactory<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyFactory<0,1,0,2>::type xyxz, rgrb, stsp;
                typename TProxyFactory<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyFactory<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyFactory<0,1,1,2>::type xyyz, rggb, sttp;
                typename TProxyFactory<0,1,2,0>::type xyzx, rgbr, stps;
                typename TProxyFactory<0,1,2,1>::type xyzy, rgbg, stpt;
                typename TProxyFactory<0,1,2,2>::type xyzz, rgbb, stpp;
                typename TProxyFactory<0,2,0,0>::type xzxx, rbrr, spss;
                typename TProxyFactory<0,2,0,1>::type xzxy, rbrg, spst;
                typename TProxyFactory<0,2,0,2>::type xzxz, rbrb, spsp;
                typename TProxyFactory<0,2,1,0>::type xzyx, rbgr, spts;
                typename TProxyFactory<0,2,1,1>::type xzyy, rbgg, sptt;
                typename TProxyFactory<0,2,1,2>::type xzyz, rbgb, sptp;
                typename TProxyFactory<0,2,2,0>::type xzzx, rbbr, spps;
                typename TProxyFactory<0,2,2,1>::type xzzy, rbbg, sppt;
                typename TProxyFactory<0,2,2,2>::type xzzz, rbbb, sppp;
                typename TProxyFactory<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyFactory<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyFactory<1,0,0,2>::type yxxz, grrb, tssp;
                typename TProxyFactory<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyFactory<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyFactory<1,0,1,2>::type yxyz, grgb, tstp;
                typename TProxyFactory<1,0,2,0>::type yxzx, grbr, tsps;
                typename TProxyFactory<1,0,2,1>::type yxzy, grbg, tspt;
                typename TProxyFactory<1,0,2,2>::type yxzz, grbb, tspp;
                typename TProxyFactory<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyFactory<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyFactory<1,1,0,2>::type yyxz, ggrb, ttsp;
                typename TProxyFactory<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyFactory<1,1,1,1>::type yyyy, gggg, tttt;
                typename TProxyFactory<1,1,1,2>::type yyyz, gggb, tttp;
                typename TProxyFactory<1,1,2,0>::type yyzx, ggbr, ttps;
                typename TProxyFactory<1,1,2,1>::type yyzy, ggbg, ttpt;
                typename TProxyFactory<1,1,2,2>::type yyzz, ggbb, ttpp;
                typename TProxyFactory<1,2,0,0>::type yzxx, gbrr, tpss;
                typename TProxyFactory<1,2,0,1>::type yzxy, gbrg, tpst;
                typename TProxyFactory<1,2,0,2>::type yzxz, gbrb, tpsp;
                typename TProxyFactory<1,2,1,0>::type yzyx, gbgr, tpts;
                typename TProxyFactory<1,2,1,1>::type yzyy, gbgg, tptt;
                typename TProxyFactory<1,2,1,2>::type yzyz, gbgb, tptp;
                typename TProxyFactory<1,2,2,0>::type yzzx, gbbr, tpps;
                typename TProxyFactory<1,2,2,1>::type yzzy, gbbg, tppt;
                typename TProxyFactory<1,2,2,2>::type yzzz, gbbb, tppp;
                typename TProxyFactory<2,0,0,0>::type zxxx, brrr, psss;
                typename TProxyFactory<2,0,0,1>::type zxxy, brrg, psst;
                typename TProxyFactory<2,0,0,2>::type zxxz, brrb, pssp;
                typename TProxyFactory<2,0,1,0>::type zxyx, brgr, psts;
                typename TProxyFactory<2,0,1,1>::type zxyy, brgg, pstt;
                typename TProxyFactory<2,0,1,2>::type zxyz, brgb, pstp;
                typename TProxyFactory<2,0,2,0>::type zxzx, brbr, psps;
                typename TProxyFactory<2,0,2,1>::type zxzy, brbg, pspt;
                typename TProxyFactory<2,0,2,2>::type zxzz, brbb, pspp;
                typename TProxyFactory<2,1,0,0>::type zyxx, bgrr, ptss;
                typename TProxyFactory<2,1,0,1>::type zyxy, bgrg, ptst;
                typename TProxyFactory<2,1,0,2>::type zyxz, bgrb, ptsp;
                typename TProxyFactory<2,1,1,0>::type zyyx, bggr, ptts;
                typename TProxyFactory<2,1,1,1>::type zyyy, bggg, pttt;
                typename TProxyFactory<2,1,1,2>::type zyyz, bggb, pttp;
                typename TProxyFactory<2,1,2,0>::type zyzx, bgbr, ptps;
                typename TProxyFactory<2,1,2,1>::type zyzy, bgbg, ptpt;
                typename TProxyFactory<2,1,2,2>::type zyzz, bgbb, ptpp;
                typename TProxyFactory<2,2,0,0>::type zzxx, bbrr, ppss;
                typename TProxyFactory<2,2,0,1>::type zzxy, bbrg, ppst;
                typename TProxyFactory<2,2,0,2>::type zzxz, bbrb, ppsp;
                typename TProxyFactory<2,2,1,0>::type zzyx, bbgr, ppts;
                typename TProxyFactory<2,2,1,1>::type zzyy, bbgg, pptt;
                typename TProxyFactory<2,2,1,2>::type zzyz, bbgb, pptp;
                typename TProxyFactory<2,2,2,0>::type zzzx, bbbr, ppps;
                typename TProxyFactory<2,2,2,1>::type zzzy, bbbg, pppt;
                typename TProxyFactory<2,2,2,2>::type zzzz, bbbb, pppp;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 4, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 4;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };*/

                typename TProxyFactory<0>::type x, r, s;
                typename TProxyFactory<1>::type y, g, t;
                typename TProxyFactory<2>::type z, b, p;
                typename TProxyFactory<3>::type w, a, q;
                typename TProxyFactory<0,0>::type xx, rr, ss;
                typename TProxyFactory<0,1>::type xy, rg, st;
                typename TProxyFactory<0,2>::type xz, rb, sp;
                typename TProxyFactory<0,3>::type xw, ra, sq;
                typename TProxyFactory<1,0>::type yx, gr, ts;
                typename TProxyFactory<1,1>::type yy, gg, tt;
                typename TProxyFactory<1,2>::type yz, gb, tp;
                typename TProxyFactory<1,3>::type yw, ga, tq;
                typename TProxyFactory<2,0>::type zx, br, ps;
                typename TProxyFactory<2,1>::type zy, bg, pt;
                typename TProxyFactory<2,2>::type zz, bb, pp;
                typename TProxyFactory<2,3>::type zw, ba, pq;
                typename TProxyFactory<3,0>::type wx, ar, qs;
                typename TProxyFactory<3,1>::type wy, ag, qt;
                typename TProxyFactory<3,2>::type wz, ab, qp;
                typename TProxyFactory<3,3>::type ww, aa, qq;
                typename TProxyFactory<0,0,0>::type xxx, rrr, sss;
                typename TProxyFactory<0,0,1>::type xxy, rrg, sst;
                typename TProxyFactory<0,0,2>::type xxz, rrb, ssp;
                typename TProxyFactory<0,0,3>::type xxw, rra, ssq;
                typename TProxyFactory<0,1,0>::type xyx, rgr, sts;
                typename TProxyFactory<0,1,1>::type xyy, rgg, stt;
                typename TProxyFactory<0,1,2>::type xyz, rgb, stp;
                typename TProxyFactory<0,1,3>::type xyw, rga, stq;
                typename TProxyFactory<0,2,0>::type xzx, rbr, sps;
                typename TProxyFactory<0,2,1>::type xzy, rbg, spt;
                typename TProxyFactory<0,2,2>::type xzz, rbb, spp;
                typename TProxyFactory<0,2,3>::type xzw, rba, spq;
                typename TProxyFactory<0,3,0>::type xwx, rar, sqs;
                typename TProxyFactory<0,3,1>::type xwy, rag, sqt;
                typename TProxyFactory<0,3,2>::type xwz, rab, sqp;
                typename TProxyFactory<0,3,3>::type xww, raa, sqq;
                typename TProxyFactory<1,0,0>::type yxx, grr, tss;
                typename TProxyFactory<1,0,1>::type yxy, grg, tst;
                typename TProxyFactory<1,0,2>::type yxz, grb, tsp;
                typename TProxyFactory<1,0,3>::type yxw, gra, tsq;
                typename TProxyFactory<1,1,0>::type yyx, ggr, tts;
                typename TProxyFactory<1,1,1>::type yyy, ggg, ttt;
                typename TProxyFactory<1,1,2>::type yyz, ggb, ttp;
                typename TProxyFactory<1,1,3>::type yyw, gga, ttq;
                typename TProxyFactory<1,2,0>::type yzx, gbr, tps;
                typename TProxyFactory<1,2,1>::type yzy, gbg, tpt;
                typename TProxyFactory<1,2,2>::type yzz, gbb, tpp;
                typename TProxyFactory<1,2,3>::type yzw, gba, tpq;
                typename TProxyFactory<1,3,0>::type ywx, gar, tqs;
                typename TProxyFactory<1,3,1>::type ywy, gag, tqt;
                typename TProxyFactory<1,3,2>::type ywz, gab, tqp;
                typename TProxyFactory<1,3,3>::type yww, gaa, tqq;
                typename TProxyFactory<2,0,0>::type zxx, brr, pss;
                typename TProxyFactory<2,0,1>::type zxy, brg, pst;
                typename TProxyFactory<2,0,2>::type zxz, brb, psp;
                typename TProxyFactory<2,0,3>::type zxw, bra, psq;
                typename TProxyFactory<2,1,0>::type zyx, bgr, pts;
                typename TProxyFactory<2,1,1>::type zyy, bgg, ptt;
                typename TProxyFactory<2,1,2>::type zyz, bgb, ptp;
                typename TProxyFactory<2,1,3>::type zyw, bga, ptq;
                typename TProxyFactory<2,2,0>::type zzx, bbr, pps;
                typename TProxyFactory<2,2,1>::type zzy, bbg, ppt;
                typename TProxyFactory<2,2,2>::type zzz, bbb, ppp;
                typename TProxyFactory<2,2,3>::type zzw, bba, ppq;
                typename TProxyFactory<2,3,0>::type zwx, bar, pqs;
                typename TProxyFactory<2,3,1>::type zwy, bag, pqt;
                typename TProxyFactory<2,3,2>::type zwz, bab, pqp;
                typename TProxyFactory<2,3,3>::type zww, baa, pqq;
                typename TProxyFactory<3,0,0>::type wxx, arr, qss;
                typename TProxyFactory<3,0,1>::type wxy, arg, qst;
                typename TProxyFactory<3,0,2>::type wxz, arb, qsp;
                typename TProxyFactory<3,0,3>::type wxw, ara, qsq;
                typename TProxyFactory<3,1,0>::type wyx, agr, qts;
                typename TProxyFactory<3,1,1>::type wyy, agg, qtt;
                typename TProxyFactory<3,1,2>::type wyz, agb, qtp;
                typename TProxyFactory<3,1,3>::type wyw, aga, qtq;
                typename TProxyFactory<3,2,0>::type wzx, abr, qps;
                typename TProxyFactory<3,2,1>::type wzy, abg, qpt;
                typename TProxyFactory<3,2,2>::type wzz, abb, qpp;
                typename TProxyFactory<3,2,3>::type wzw, aba, qpq;
                typename TProxyFactory<3,3,0>::type wwx, aar, qqs;
                typename TProxyFactory<3,3,1>::type wwy, aag, qqt;
                typename TProxyFactory<3,3,2>::type wwz, aab, qqp;
                typename TProxyFactory<3,3,3>::type www, aaa, qqq;
                typename TProxyFactory<0,0,0,0>::type xxxx, rrrr, ssss;
                typename TProxyFactory<0,0,0,1>::type xxxy, rrrg, ssst;
                typename TProxyFactory<0,0,0,2>::type xxxz, rrrb, sssp;
                typename TProxyFactory<0,0,0,3>::type xxxw, rrra, sssq;
                typename TProxyFactory<0,0,1,0>::type xxyx, rrgr, ssts;
                typename TProxyFactory<0,0,1,1>::type xxyy, rrgg, sstt;
                typename TProxyFactory<0,0,1,2>::type xxyz, rrgb, sstp;
                typename TProxyFactory<0,0,1,3>::type xxyw, rrga, sstq;
                typename TProxyFactory<0,0,2,0>::type xxzx, rrbr, ssps;
                typename TProxyFactory<0,0,2,1>::type xxzy, rrbg, sspt;
                typename TProxyFactory<0,0,2,2>::type xxzz, rrbb, sspp;
                typename TProxyFactory<0,0,2,3>::type xxzw, rrba, sspq;
                typename TProxyFactory<0,0,3,0>::type xxwx, rrar, ssqs;
                typename TProxyFactory<0,0,3,1>::type xxwy, rrag, ssqt;
                typename TProxyFactory<0,0,3,2>::type xxwz, rrab, ssqp;
                typename TProxyFactory<0,0,3,3>::type xxww, rraa, ssqq;
                typename TProxyFactory<0,1,0,0>::type xyxx, rgrr, stss;
                typename TProxyFactory<0,1,0,1>::type xyxy, rgrg, stst;
                typename TProxyFactory<0,1,0,2>::type xyxz, rgrb, stsp;
                typename TProxyFactory<0,1,0,3>::type xyxw, rgra, stsq;
                typename TProxyFactory<0,1,1,0>::type xyyx, rggr, stts;
                typename TProxyFactory<0,1,1,1>::type xyyy, rggg, sttt;
                typename TProxyFactory<0,1,1,2>::type xyyz, rggb, sttp;
                typename TProxyFactory<0,1,1,3>::type xyyw, rgga, sttq;
                typename TProxyFactory<0,1,2,0>::type xyzx, rgbr, stps;
                typename TProxyFactory<0,1,2,1>::type xyzy, rgbg, stpt;
                typename TProxyFactory<0,1,2,2>::type xyzz, rgbb, stpp;
                typename TProxyFactory<0,1,2,3>::type xyzw, rgba, stpq;
                typename TProxyFactory<0,1,3,0>::type xywx, rgar, stqs;
                typename TProxyFactory<0,1,3,1>::type xywy, rgag, stqt;
                typename TProxyFactory<0,1,3,2>::type xywz, rgab, stqp;
                typename TProxyFactory<0,1,3,3>::type xyww, rgaa, stqq;
                typename TProxyFactory<0,2,0,0>::type xzxx, rbrr, spss;
                typename TProxyFactory<0,2,0,1>::type xzxy, rbrg, spst;
                typename TProxyFactory<0,2,0,2>::type xzxz, rbrb, spsp;
                typename TProxyFactory<0,2,0,3>::type xzxw, rbra, spsq;
                typename TProxyFactory<0,2,1,0>::type xzyx, rbgr, spts;
                typename TProxyFactory<0,2,1,1>::type xzyy, rbgg, sptt;
                typename TProxyFactory<0,2,1,2>::type xzyz, rbgb, sptp;
                typename TProxyFactory<0,2,1,3>::type xzyw, rbga, sptq;
                typename TProxyFactory<0,2,2,0>::type xzzx, rbbr, spps;
                typename TProxyFactory<0,2,2,1>::type xzzy, rbbg, sppt;
                typename TProxyFactory<0,2,2,2>::type xzzz, rbbb, sppp;
                typename TProxyFactory<0,2,2,3>::type xzzw, rbba, sppq;
                typename TProxyFactory<0,2,3,0>::type xzwx, rbar, spqs;
                typename TProxyFactory<0,2,3,1>::type xzwy, rbag, spqt;
                typename TProxyFactory<0,2,3,2>::type xzwz, rbab, spqp;
                typename TProxyFactory<0,2,3,3>::type xzww, rbaa, spqq;
                typename TProxyFactory<0,3,0,0>::type xwxx, rarr, sqss;
                typename TProxyFactory<0,3,0,1>::type xwxy, rarg, sqst;
                typename TProxyFactory<0,3,0,2>::type xwxz, rarb, sqsp;
                typename TProxyFactory<0,3,0,3>::type xwxw, rara, sqsq;
                typename TProxyFactory<0,3,1,0>::type xwyx, ragr, sqts;
                typename TProxyFactory<0,3,1,1>::type xwyy, ragg, sqtt;
                typename TProxyFactory<0,3,1,2>::type xwyz, ragb, sqtp;
                typename TProxyFactory<0,3,1,3>::type xwyw, raga, sqtq;
                typename TProxyFactory<0,3,2,0>::type xwzx, rabr, sqps;
                typename TProxyFactory<0,3,2,1>::type xwzy, rabg, sqpt;
                typename TProxyFactory<0,3,2,2>::type xwzz, rabb, sqpp;
                typename TProxyFactory<0,3,2,3>::type xwzw, raba, sqpq;
                typename TProxyFactory<0,3,3,0>::type xwwx, raar, sqqs;
                typename TProxyFactory<0,3,3,1>::type xwwy, raag, sqqt;
                typename TProxyFactory<0,3,3,2>::type xwwz, raab, sqqp;
                typename TProxyFactory<0,3,3,3>::type xwww, raaa, sqqq;
                typename TProxyFactory<1,0,0,0>::type yxxx, grrr, tsss;
                typename TProxyFactory<1,0,0,1>::type yxxy, grrg, tsst;
                typename TProxyFactory<1,0,0,2>::type yxxz, grrb, tssp;
                typename TProxyFactory<1,0,0,3>::type yxxw, grra, tssq;
                typename TProxyFactory<1,0,1,0>::type yxyx, grgr, tsts;
                typename TProxyFactory<1,0,1,1>::type yxyy, grgg, tstt;
                typename TProxyFactory<1,0,1,2>::type yxyz, grgb, tstp;
                typename TProxyFactory<1,0,1,3>::type yxyw, grga, tstq;
                typename TProxyFactory<1,0,2,0>::type yxzx, grbr, tsps;
                typename TProxyFactory<1,0,2,1>::type yxzy, grbg, tspt;
                typename TProxyFactory<1,0,2,2>::type yxzz, grbb, tspp;
                typename TProxyFactory<1,0,2,3>::type yxzw, grba, tspq;
                typename TProxyFactory<1,0,3,0>::type yxwx, grar, tsqs;
                typename TProxyFactory<1,0,3,1>::type yxwy, grag, tsqt;
                typename TProxyFactory<1,0,3,2>::type yxwz, grab, tsqp;
                typename TProxyFactory<1,0,3,3>::type yxww, graa, tsqq;
                typename TProxyFactory<1,1,0,0>::type yyxx, ggrr, ttss;
                typename TProxyFactory<1,1,0,1>::type yyxy, ggrg, ttst;
                typename TProxyFactory<1,1,0,2>::type yyxz, ggrb, ttsp;
                typename TProxyFactory<1,1,0,3>::type yyxw, ggra, ttsq;
                typename TProxyFactory<1,1,1,0>::type yyyx, gggr, ttts;
                typename TProxyFactory<1,1,1,1>::type yyyy, gggg, tttt;
                typename TProxyFactory<1,1,1,2>::type yyyz, gggb, tttp;
                typename TProxyFactory<1,1,1,3>::type yyyw, ggga, tttq;
                typename TProxyFactory<1,1,2,0>::type yyzx, ggbr, ttps;
                typename TProxyFactory<1,1,2,1>::type yyzy, ggbg, ttpt;
                typename TProxyFactory<1,1,2,2>::type yyzz, ggbb, ttpp;
                typename TProxyFactory<1,1,2,3>::type yyzw, ggba, ttpq;
                typename TProxyFactory<1,1,3,0>::type yywx, ggar, ttqs;
                typename TProxyFactory<1,1,3,1>::type yywy, ggag, ttqt;
                typename TProxyFactory<1,1,3,2>::type yywz, ggab, ttqp;
                typename TProxyFactory<1,1,3,3>::type yyww, ggaa, ttqq;
                typename TProxyFactory<1,2,0,0>::type yzxx, gbrr, tpss;
                typename TProxyFactory<1,2,0,1>::type yzxy, gbrg, tpst;
                typename TProxyFactory<1,2,0,2>::type yzxz, gbrb, tpsp;
                typename TProxyFactory<1,2,0,3>::type yzxw, gbra, tpsq;
                typename TProxyFactory<1,2,1,0>::type yzyx, gbgr, tpts;
                typename TProxyFactory<1,2,1,1>::type yzyy, gbgg, tptt;
                typename TProxyFactory<1,2,1,2>::type yzyz, gbgb, tptp;
                typename TProxyFactory<1,2,1,3>::type yzyw, gbga, tptq;
                typename TProxyFactory<1,2,2,0>::type yzzx, gbbr, tpps;
                typename TProxyFactory<1,2,2,1>::type yzzy, gbbg, tppt;
                typename TProxyFactory<1,2,2,2>::type yzzz, gbbb, tppp;
                typename TProxyFactory<1,2,2,3>::type yzzw, gbba, tppq;
                typename TProxyFactory<1,2,3,0>::type yzwx, gbar, tpqs;
                typename TProxyFactory<1,2,3,1>::type yzwy, gbag, tpqt;
                typename TProxyFactory<1,2,3,2>::type yzwz, gbab, tpqp;
                typename TProxyFactory<1,2,3,3>::type yzww, gbaa, tpqq;
                typename TProxyFactory<1,3,0,0>::type ywxx, garr, tqss;
                typename TProxyFactory<1,3,0,1>::type ywxy, garg, tqst;
                typename TProxyFactory<1,3,0,2>::type ywxz, garb, tqsp;
                typename TProxyFactory<1,3,0,3>::type ywxw, gara, tqsq;
                typename TProxyFactory<1,3,1,0>::type ywyx, gagr, tqts;
                typename TProxyFactory<1,3,1,1>::type ywyy, gagg, tqtt;
                typename TProxyFactory<1,3,1,2>::type ywyz, gagb, tqtp;
                typename TProxyFactory<1,3,1,3>::type ywyw, gaga, tqtq;
                typename TProxyFactory<1,3,2,0>::type ywzx, gabr, tqps;
                typename TProxyFactory<1,3,2,1>::type ywzy, gabg, tqpt;
                typename TProxyFactory<1,3,2,2>::type ywzz, gabb, tqpp;
                typename TProxyFactory<1,3,2,3>::type ywzw, gaba, tqpq;
                typename TProxyFactory<1,3,3,0>::type ywwx, gaar, tqqs;
                typename TProxyFactory<1,3,3,1>::type ywwy, gaag, tqqt;
                typename TProxyFactory<1,3,3,2>::type ywwz, gaab, tqqp;
                typename TProxyFactory<1,3,3,3>::type ywww, gaaa, tqqq;
                typename TProxyFactory<2,0,0,0>::type zxxx, brrr, psss;
                typename TProxyFactory<2,0,0,1>::type zxxy, brrg, psst;
                typename TProxyFactory<2,0,0,2>::type zxxz, brrb, pssp;
                typename TProxyFactory<2,0,0,3>::type zxxw, brra, pssq;
                typename TProxyFactory<2,0,1,0>::type zxyx, brgr, psts;
                typename TProxyFactory<2,0,1,1>::type zxyy, brgg, pstt;
                typename TProxyFactory<2,0,1,2>::type zxyz, brgb, pstp;
                typename TProxyFactory<2,0,1,3>::type zxyw, brga, pstq;
                typename TProxyFactory<2,0,2,0>::type zxzx, brbr, psps;
                typename TProxyFactory<2,0,2,1>::type zxzy, brbg, pspt;
                typename TProxyFactory<2,0,2,2>::type zxzz, brbb, pspp;
                typename TProxyFactory<2,0,2,3>::type zxzw, brba, pspq;
                typename TProxyFactory<2,0,3,0>::type zxwx, brar, psqs;
                typename TProxyFactory<2,0,3,1>::type zxwy, brag, psqt;
                typename TProxyFactory<2,0,3,2>::type zxwz, brab, psqp;
                typename TProxyFactory<2,0,3,3>::type zxww, braa, psqq;
                typename TProxyFactory<2,1,0,0>::type zyxx, bgrr, ptss;
                typename TProxyFactory<2,1,0,1>::type zyxy, bgrg, ptst;
                typename TProxyFactory<2,1,0,2>::type zyxz, bgrb, ptsp;
                typename TProxyFactory<2,1,0,3>::type zyxw, bgra, ptsq;
                typename TProxyFactory<2,1,1,0>::type zyyx, bggr, ptts;
                typename TProxyFactory<2,1,1,1>::type zyyy, bggg, pttt;
                typename TProxyFactory<2,1,1,2>::type zyyz, bggb, pttp;
                typename TProxyFactory<2,1,1,3>::type zyyw, bgga, pttq;
                typename TProxyFactory<2,1,2,0>::type zyzx, bgbr, ptps;
                typename TProxyFactory<2,1,2,1>::type zyzy, bgbg, ptpt;
                typename TProxyFactory<2,1,2,2>::type zyzz, bgbb, ptpp;
                typename TProxyFactory<2,1,2,3>::type zyzw, bgba, ptpq;
                typename TProxyFactory<2,1,3,0>::type zywx, bgar, ptqs;
                typename TProxyFactory<2,1,3,1>::type zywy, bgag, ptqt;
                typename TProxyFactory<2,1,3,2>::type zywz, bgab, ptqp;
                typename TProxyFactory<2,1,3,3>::type zyww, bgaa, ptqq;
                typename TProxyFactory<2,2,0,0>::type zzxx, bbrr, ppss;
                typename TProxyFactory<2,2,0,1>::type zzxy, bbrg, ppst;
                typename TProxyFactory<2,2,0,2>::type zzxz, bbrb, ppsp;
                typename TProxyFactory<2,2,0,3>::type zzxw, bbra, ppsq;
                typename TProxyFactory<2,2,1,0>::type zzyx, bbgr, ppts;
                typename TProxyFactory<2,2,1,1>::type zzyy, bbgg, pptt;
                typename TProxyFactory<2,2,1,2>::type zzyz, bbgb, pptp;
                typename TProxyFactory<2,2,1,3>::type zzyw, bbga, pptq;
                typename TProxyFactory<2,2,2,0>::type zzzx, bbbr, ppps;
                typename TProxyFactory<2,2,2,1>::type zzzy, bbbg, pppt;
                typename TProxyFactory<2,2,2,2>::type zzzz, bbbb, pppp;
                typename TProxyFactory<2,2,2,3>::type zzzw, bbba, pppq;
                typename TProxyFactory<2,2,3,0>::type zzwx, bbar, ppqs;
                typename TProxyFactory<2,2,3,1>::type zzwy, bbag, ppqt;
                typename TProxyFactory<2,2,3,2>::type zzwz, bbab, ppqp;
                typename TProxyFactory<2,2,3,3>::type zzww, bbaa, ppqq;
                typename TProxyFactory<2,3,0,0>::type zwxx, barr, pqss;
                typename TProxyFactory<2,3,0,1>::type zwxy, barg, pqst;
                typename TProxyFactory<2,3,0,2>::type zwxz, barb, pqsp;
                typename TProxyFactory<2,3,0,3>::type zwxw, bara, pqsq;
                typename TProxyFactory<2,3,1,0>::type zwyx, bagr, pqts;
                typename TProxyFactory<2,3,1,1>::type zwyy, bagg, pqtt;
                typename TProxyFactory<2,3,1,2>::type zwyz, bagb, pqtp;
                typename TProxyFactory<2,3,1,3>::type zwyw, baga, pqtq;
                typename TProxyFactory<2,3,2,0>::type zwzx, babr, pqps;
                typename TProxyFactory<2,3,2,1>::type zwzy, babg, pqpt;
                typename TProxyFactory<2,3,2,2>::type zwzz, babb, pqpp;
                typename TProxyFactory<2,3,2,3>::type zwzw, baba, pqpq;
                typename TProxyFactory<2,3,3,0>::type zwwx, baar, pqqs;
                typename TProxyFactory<2,3,3,1>::type zwwy, baag, pqqt;
                typename TProxyFactory<2,3,3,2>::type zwwz, baab, pqqp;
                typename TProxyFactory<2,3,3,3>::type zwww, baaa, pqqq;
                typename TProxyFactory<3,0,0,0>::type wxxx, arrr, qsss;
                typename TProxyFactory<3,0,0,1>::type wxxy, arrg, qsst;
                typename TProxyFactory<3,0,0,2>::type wxxz, arrb, qssp;
                typename TProxyFactory<3,0,0,3>::type wxxw, arra, qssq;
                typename TProxyFactory<3,0,1,0>::type wxyx, argr, qsts;
                typename TProxyFactory<3,0,1,1>::type wxyy, argg, qstt;
                typename TProxyFactory<3,0,1,2>::type wxyz, argb, qstp;
                typename TProxyFactory<3,0,1,3>::type wxyw, arga, qstq;
                typename TProxyFactory<3,0,2,0>::type wxzx, arbr, qsps;
                typename TProxyFactory<3,0,2,1>::type wxzy, arbg, qspt;
                typename TProxyFactory<3,0,2,2>::type wxzz, arbb, qspp;
                typename TProxyFactory<3,0,2,3>::type wxzw, arba, qspq;
                typename TProxyFactory<3,0,3,0>::type wxwx, arar, qsqs;
                typename TProxyFactory<3,0,3,1>::type wxwy, arag, qsqt;
                typename TProxyFactory<3,0,3,2>::type wxwz, arab, qsqp;
                typename TProxyFactory<3,0,3,3>::type wxww, araa, qsqq;
                typename TProxyFactory<3,1,0,0>::type wyxx, agrr, qtss;
                typename TProxyFactory<3,1,0,1>::type wyxy, agrg, qtst;
                typename TProxyFactory<3,1,0,2>::type wyxz, agrb, qtsp;
                typename TProxyFactory<3,1,0,3>::type wyxw, agra, qtsq;
                typename TProxyFactory<3,1,1,0>::type wyyx, aggr, qtts;
                typename TProxyFactory<3,1,1,1>::type wyyy, aggg, qttt;
                typename TProxyFactory<3,1,1,2>::type wyyz, aggb, qttp;
                typename TProxyFactory<3,1,1,3>::type wyyw, agga, qttq;
                typename TProxyFactory<3,1,2,0>::type wyzx, agbr, qtps;
                typename TProxyFactory<3,1,2,1>::type wyzy, agbg, qtpt;
                typename TProxyFactory<3,1,2,2>::type wyzz, agbb, qtpp;
                typename TProxyFactory<3,1,2,3>::type wyzw, agba, qtpq;
                typename TProxyFactory<3,1,3,0>::type wywx, agar, qtqs;
                typename TProxyFactory<3,1,3,1>::type wywy, agag, qtqt;
                typename TProxyFactory<3,1,3,2>::type wywz, agab, qtqp;
                typename TProxyFactory<3,1,3,3>::type wyww, agaa, qtqq;
                typename TProxyFactory<3,2,0,0>::type wzxx, abrr, qpss;
                typename TProxyFactory<3,2,0,1>::type wzxy, abrg, qpst;
                typename TProxyFactory<3,2,0,2>::type wzxz, abrb, qpsp;
                typename TProxyFactory<3,2,0,3>::type wzxw, abra, qpsq;
                typename TProxyFactory<3,2,1,0>::type wzyx, abgr, qpts;
                typename TProxyFactory<3,2,1,1>::type wzyy, abgg, qptt;
                typename TProxyFactory<3,2,1,2>::type wzyz, abgb, qptp;
                typename TProxyFactory<3,2,1,3>::type wzyw, abga, qptq;
                typename TProxyFactory<3,2,2,0>::type wzzx, abbr, qpps;
                typename TProxyFactory<3,2,2,1>::type wzzy, abbg, qppt;
                typename TProxyFactory<3,2,2,2>::type wzzz, abbb, qppp;
                typename TProxyFactory<3,2,2,3>::type wzzw, abba, qppq;
                typename TProxyFactory<3,2,3,0>::type wzwx, abar, qpqs;
                typename TProxyFactory<3,2,3,1>::type wzwy, abag, qpqt;
                typename TProxyFactory<3,2,3,2>::type wzwz, abab, qpqp;
                typename TProxyFactory<3,2,3,3>::type wzww, abaa, qpqq;
                typename TProxyFactory<3,3,0,0>::type wwxx, aarr, qqss;
                typename TProxyFactory<3,3,0,1>::type wwxy, aarg, qqst;
                typename TProxyFactory<3,3,0,2>::type wwxz, aarb, qqsp;
                typename TProxyFactory<3,3,0,3>::type wwxw, aara, qqsq;
                typename TProxyFactory<3,3,1,0>::type wwyx, aagr, qqts;
                typename TProxyFactory<3,3,1,1>::type wwyy, aagg, qqtt;
                typename TProxyFactory<3,3,1,2>::type wwyz, aagb, qqtp;
                typename TProxyFactory<3,3,1,3>::type wwyw, aaga, qqtq;
                typename TProxyFactory<3,3,2,0>::type wwzx, aabr, qqps;
                typename TProxyFactory<3,3,2,1>::type wwzy, aabg, qqpt;
                typename TProxyFactory<3,3,2,2>::type wwzz, aabb, qqpp;
                typename TProxyFactory<3,3,2,3>::type wwzw, aaba, qqpq;
                typename TProxyFactory<3,3,3,0>::type wwwx, aaar, qqqs;
                typename TProxyFactory<3,3,3,1>::type wwwy, aaag, qqqt;
                typename TProxyFactory<3,3,3,2>::type wwwz, aaab, qqqp;
                typename TProxyFactory<3,3,3,3>::type wwww, aaaa, qqqq;


            };
        };
    }
}