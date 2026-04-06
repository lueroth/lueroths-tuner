//
// Created by lueroth on 02.03.2026.
//

#include "YINPitchDetection.h"

#include <cmath>
#include <comcat.h>
#include <stdexcept>
#include <string>

using namespace std;

namespace yin {
    YINPitchDetection::YINPitchDetection(size_t bufferSize, double sampleRate, float threshold) :

    bufferSize_(bufferSize),
    sampleRate_(sampleRate),
    threshold_(threshold),
    diff_(bufferSize / 2, 0.0f) {

        if (sampleRate_ <= 0.0f) {
            throw invalid_argument("sampleRate must be greater than 0");
        }
        if (bufferSize_ < 2) {
            throw invalid_argument("bufferSize must be at least 2 for proper period calculation");
        }
    }

    void YINPitchDetection::setSampleRate(double sampleRate) {

        if (sampleRate <= 0.0f) {
            throw invalid_argument("sampleRate must be greater than 0");
        }
        sampleRate_ = sampleRate;
    }
    void YINPitchDetection::setThreshold(float threshold) {
        threshold_ = threshold;
    }
    double YINPitchDetection::getSampleRate() const {
        return sampleRate_;
    }
    float YINPitchDetection::getThreshold() const {
        return threshold_;
    }

    void YINPitchDetection::difference(const float *samples) {

        const size_t size = diff_.size();

        for (size_t tau = 1; tau < size; ++tau) {
            diff_[tau] = 0.0f;

            for (size_t i = 0; i < size; ++i) {
                const float diff = samples[i] - samples[tau + i];
                diff_[tau] += diff * diff;
            }
        }
    }

    void YINPitchDetection::CMND() {
        const size_t size = diff_.size();
        diff_[0] = 1.0f;
        float sum = 0.0f;

        for (size_t tau = 1; tau < size; ++tau) {
            sum += diff_[tau];
            if (sum == 0.0f) {
                diff_[tau] = 0.0f;
            } else {
                diff_[tau] *= static_cast<float>(tau) / sum;
            }
        }
    }

    float YINPitchDetection::absoluteThreshold() const {
        const size_t size = diff_.size();

        for (size_t tau = 2; tau < size; ++tau) {
            if (diff_[tau] < threshold_) {
                while (tau + 1 < size && diff_[tau + 1] < diff_[tau]) {
                    ++tau;
                }
                return interpolation(tau);
            }
        }
        return NoPitch;
    }

    float YINPitchDetection::interpolation(size_t tauEstimate) const {
        const size_t size = diff_.size();
        if (tauEstimate == 0 || tauEstimate + 1 >= size) {
            return static_cast<float>(tauEstimate);
        }

        const float y1 = diff_[tauEstimate - 1];
        const float y2 = diff_[tauEstimate];
        const float y3 = diff_[tauEstimate + 1];
        const float denom = 2.0f * (2.0f * y2 - y3 - y1);

        if (std::abs(denom) < 1e-7f) {
            return static_cast<float>(tauEstimate);
        }

        return static_cast<float>(tauEstimate) + (y3 - y1) / denom;
    }

    double YINPitchDetection::detect(const float *samples, size_t count) {
        if (count < bufferSize_) {
            return NoPitch;
        }

        difference(samples);

        if (diff_[1] < 1e-4f) return NoPitch;

        CMND();

        const float tau = absoluteThreshold();
        if (tau < 0.0f) {
            return NoPitch;
        }

        return sampleRate_ / tau;
    }
}
