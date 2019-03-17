#pragma once

#include <afxwin.h>

#include <util/common/geom/geom.h>
#include <util/common/plot/plot.h>

namespace model
{

    using points_t = std::vector < geom::point2d_t > ;

    struct parameters
    {
    };

    inline static parameters make_default_parameters()
    {
        return
        {
        };
    }

    struct plot_data
    {
        plot::simple_list_plot < points_t > plot;
    };

    struct model_data
    {
        util::ptr_t < parameters > params;
        plot_data sine_data;
    };

    inline static plot_data make_plot_data
    (
        plot::palette::pen_ptr pen = plot::palette::pen(0x000000)
    )
    {
        auto vp = plot::min_max_auto_viewport < points_t > ::create();
        auto params = vp->get_params();
        params.factors = { 0, 0, 0.1, 0.1 };
        vp->set_params(params);
        plot_data pd;
        pd.plot
            .with_view()
            .with_view_line_pen(pen)
            .with_auto_viewport(std::move(vp))
            .with_data()
            ;
        pd.plot.view->data_format = plot::list_data_format::chain;
        return pd;
    }

    inline static plot::drawable::ptr_t make_root_drawable
    (
        plot::world_mapper_t world_mapper,
        std::vector < plot::drawable::ptr_t > layers
    )
    {
        using namespace plot;

        return viewporter::create(
            tick_drawable::create(
                layer_drawable::create(layers),
                const_n_tick_factory<axe::x>::create(
                    make_simple_tick_formatter(2, 5),
                    0,
                    5
                ),
                const_n_tick_factory<axe::y>::create(
                    make_simple_tick_formatter(2, 5),
                    0,
                    5
                ),
                palette::pen(RGB(80, 80, 80)),
                RGB(200, 200, 200)
            ),
            make_viewport_mapper(world_mapper)
        );
    }

    inline model_data make_model_data()
    {
        return {
            util::create < parameters > (make_default_parameters()),
            make_plot_data()
        };
    }

    class sine_generator
    {
    public:
        struct cfg_t
        {
            double mag, freq, dfreq, per, dt;
            bool operator == (const cfg_t & o) {
                return mag == o.mag && freq == o.freq && dfreq == o.dfreq
                    && per == o.per && dt == o.dt; }
        };
        struct state_t
        {
        private:
            cfg_t cfg;
            size_t n, N;
            double phi;
            double t;
            double T;
            friend class sine_generator;
        };
        using callback_t = std::function < void (size_t i, geom::point2d_t v) > ;
    private:
        state_t state;
    public:
        const state_t get_state() const { return state; }
        void set_state(const state_t & state) { this->state = state; }
    private:
        cfg_t cfg;
    public:
        const cfg_t get_cfg() const { return cfg; }
    public:
        sine_generator()
            : cfg({ 1, 0.1, 0, 10, 0.1 })
        {
            state.cfg = cfg;
            state.n = 0;
            state.t = 0;
            state.N = 0;
            state.T = 0;
            state.phi = 0;
        }
    public:
        void next(const cfg_t & cfg, size_t n, callback_t callback)
        {
            _reconfig(cfg);
            next(n, callback);
        }
        void next(size_t n, callback_t callback)
        {
            for (size_t i = 0; i < n; ++i)
            {
                callback(this->state.N, next());
            }
        }
        geom::point2d_t next(const cfg_t & cfg)
        {
            _reconfig(cfg);
            return next();
        }
        geom::point2d_t next()
        {
            double t = this->state.T;
            double r = _cur();
            ++this->state.n;
            ++this->state.N;
            this->state.t += this->state.cfg.dt;
            this->state.T += this->state.cfg.dt;
            return { t, r };
        }
    private:
        double _cur() const
        {
            return this->state.cfg.mag * std::sin(_cur_phase());
        }
        double _cur_phase() const
        {
            double period = this->state.cfg.per;
            double t = this->state.t;
            if (period == 0) return 2 * M_PI * this->state.cfg.freq * this->state.t + this->state.phi;
            int n = (int) floor(t / period);
            t -= n * period;
            double lfm = 0;
            if ((n % 2) == 0)
            {
                lfm += (t * t / period / 2 - t / 2);
            }
            else
            {
                lfm += (t / 2 - t * t / period / 2);
            }
            return 2 * M_PI * (lfm * this->state.cfg.dfreq + this->state.t * this->state.cfg.freq) + this->state.phi;
        }
        void _reconfig(const cfg_t & cfg)
        {
            this->cfg = cfg;
            if (!(this->cfg == this->state.cfg))
            {
                this->state.phi = _cur_phase();
                this->state.cfg = cfg;
                this->state.n = 0;
                this->state.t = 0;
            }
        }
    };
}
