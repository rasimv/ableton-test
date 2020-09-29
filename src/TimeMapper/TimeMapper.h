#include <cassert>
#include <map>
#include <any>

class TimeMapper
{
    // std::any is used to solve declaration loop. Causes some overhead though
    using B2s = std::map<double, std::any>;
    using S2b = std::map<double, std::any>;

public:
    class OutOfRange : public std::exception {};

    // Add the marker <beat, sample>
    // Overrides (removes) previously added markers which intersect the new one
    void addMarker(double beat, double sample)
    {
        assert(checkConsistency());

        const auto testBeat(m_b2s.equal_range(beat));
        if (testBeat.first != std::end(m_b2s))
        {
            const auto testSample(cast2S(testBeat.first->second));
            if (sample >= testSample->first)
            {
                const auto endSample(m_s2b.upper_bound(sample));
                const auto endBeat(endSample == std::end(m_s2b) ?
                    std::end(m_b2s) : cast2B(endSample->second));
                change(testBeat.first, endBeat, testSample, endSample, beat, sample);
                return;
            }
        }

        const auto beginSample(m_s2b.lower_bound(sample));
        if (beginSample == std::end(m_s2b))
        {
            change(std::end(m_b2s), std::end(m_b2s), std::end(m_s2b), std::end(m_s2b), beat, sample);
            return;
        }

        const auto beginBeat(cast2B(beginSample->second));
        const auto endSample(testBeat.second == std::end(m_b2s) ?
            std::end(m_s2b) : cast2S(testBeat.second->second));
        change(beginBeat, testBeat.second, beginSample, endSample, beat, sample);
    }

    void setEndTempo(double tempo)
    {
        if (tempo <= 0) throw OutOfRange();
        m_endTempo = tempo;
    }

    // Returns the sample time by the given beat time
    double sample(double beat) const
    {
        assert(checkConsistency());

        if (m_b2s.empty()) return beat / m_endTempo;

        const auto ubBeat(m_b2s.upper_bound(beat));
        if (ubBeat == std::end(m_b2s))
        {
            const auto lastBeat(std::rbegin(m_b2s));
            const auto lastSample(std::rbegin(m_s2b));
            return lastSample->first + (beat - lastBeat->first) / m_endTempo;
        }

        const auto ubSample(cast2S(ubBeat->second));
        if (ubBeat == std::begin(m_b2s))
        {
            const auto nextBeat(next(ubBeat));
            const auto tempo = nextBeat == std::end(m_b2s) ? m_endTempo :
                (nextBeat->first - ubBeat->first) / (cast2S(nextBeat->second)->first - ubSample->first);
            return ubSample->first - (ubBeat->first - beat) / tempo;
        }

        const auto prevBeat(prev(ubBeat));
        const auto prevSample(prev(ubSample));
        const auto tempo = (ubBeat->first - prevBeat->first) / (ubSample->first - prevSample->first);
        return prevSample->first + (beat - prevBeat->first) / tempo;
    }

    // Returns the beat time by the given sample time
    double beat(double sample) const
    {
        assert(checkConsistency());

        if (m_s2b.empty()) return sample * m_endTempo;

        const auto ubSample(m_s2b.upper_bound(sample));
        if (ubSample == std::end(m_s2b))
        {
            const auto lastBeat(std::rbegin(m_b2s));
            const auto lastSample(std::rbegin(m_s2b));
            return lastBeat->first + (sample - lastSample->first) * m_endTempo;
        }

        const auto ubBeat(cast2B(ubSample->second));
        if (ubSample == std::begin(m_s2b))
        {
            const auto nextSample(next(ubSample));
            const auto tempo = nextSample == std::end(m_s2b) ? m_endTempo :
                (cast2B(nextSample->second)->first - ubBeat->first) / (nextSample->first - ubSample->first);
            return ubBeat->first - (ubSample->first - sample) * tempo;
        }

        const auto prevBeat(prev(ubBeat));
        const auto prevSample(prev(ubSample));
        const auto tempo = (ubBeat->first - prevBeat->first) / (ubSample->first - prevSample->first);
        return prevBeat->first + (sample - prevSample->first) * tempo;
    }

private:
    template <typename It> It prev(It it) const { return --it; }
    template <typename It> It next(It it) const { return ++it; }

    template <typename Any>
    B2s::iterator cast2B(Any any) const { return std::any_cast<B2s::iterator>(any); }
    template <typename Any>
    S2b::iterator cast2S(Any any) const { return std::any_cast<S2b::iterator>(any); }

    // Erases [bb, eb) & [bs, es) ranges from the maps and insert the new pair
    template <typename B2sIt, typename S2bIt>
    void change(B2sIt bb, B2sIt eb, S2bIt bs, S2bIt es, double beat, double sample)
    {
        assert(checkConsistency());

        m_b2s.erase(bb, eb);
        m_s2b.erase(bs, es);

        const auto result(m_b2s.emplace(beat, 0));
        try { result.first->second = m_s2b.emplace(sample, result.first).first; }
        catch (const std::exception &) { m_b2s.erase(result.first); throw; }
    }

    // A check method. Will not present in the release build (because is used only with asserts)
    bool checkConsistency() const
    {
        if (m_b2s.size() != m_s2b.size()) return false;
        auto i(std::begin(m_b2s));
        for (auto j(std::begin(m_s2b)); j != std::end(m_s2b); ++i, ++j)
            if (i != cast2B(j->second) || j != cast2S(i->second)) return false;
        return true;
    }

    B2s m_b2s;
    S2b m_s2b;
    double m_endTempo = 10;
};
