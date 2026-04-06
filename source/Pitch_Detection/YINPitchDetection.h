//
// Created by lueroth on 02.03.2026.
//

#ifndef YINPITCHDETECTION_H
#define YINPITCHDETECTION_H

#include <vector>

namespace yin {

    static constexpr float DefaultThreshold = 0.15f;
    static constexpr float NoPitch = -1.0f;

    class YINPitchDetection {

    public:

        std::size_t bufferSize_;
        double sampleRate_;
        float threshold_;

        explicit YINPitchDetection(std::size_t bufferSize, double sampleRate ,float threshold = DefaultThreshold);
        double detect(const float* samples ,std::size_t count);

        void setSampleRate(double sampleRate);
        void setThreshold(float threshold);

        [[nodiscard]] double getSampleRate() const;
        [[nodiscard]] float getThreshold() const;

    private:

        void difference(const float* samples);
        void CMND();
        [[nodiscard]] float absoluteThreshold() const;
        [[nodiscard]] float interpolation(std::size_t tauEstimate) const;

        std::vector<float> diff_;
    };
}

#endif //YINPITCHDETECTION_H