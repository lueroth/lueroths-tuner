//
// Created by lueroth on 24.03.2026.
//
#include "FindNote_n_Offset.h"

#include <string>
#include <array>
#include "YINPitchDetection.h"

std::array<std::string, 13> names {"A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A "};

namespace fno {
    FindNote_n_Offset::FindNote_n_Offset(float refFreq) :
    refFreq_(refFreq) {}

    void FindNote_n_Offset::setSampleRate(double sampleRate) {
        detection.setSampleRate(sampleRate);
    }

    void FindNote_n_Offset::setRefFreq(double refFreq) {
        refFreq_ = refFreq;
    }

    double FindNote_n_Offset::getRefFreq() const {
        return refFreq_;
    }

    note_n_offset FindNote_n_Offset::findNote(const float *samples, size_t count) {
        // get frequency
        double freq = detection.detect(samples, count);

        // detect invalid frequency
        if (freq <= 0.0f) return {"  ", 0.5f};

        // calculate offset from reference
        double cents = 1200 * log2(freq / refFreq_);

        // copy of cents for proper calculations
        double unsCents = abs(cents);
        bool reverse {false};

        // detect reverse order
        if (cents < 0) reverse = true;

        int note {};
        note_n_offset result {};

        // there is 9 octaves, but because of reference note this number doesn't make sense
        // does not need fixing, the number is high enough
        for (int n = 0; n <= 9; n++) {

            // 12 notes. 13th note in the array is purely for reverse order.
            for (int m = 0; m <= 11; m++) {
                // finding note
                if (abs(unsCents - note) <= 50 || abs(note - unsCents) <= 50) {
                    // if freq < reference we count notes in reverse order
                    if (reverse) {
                        result = {names[12 - m], (static_cast<float>(note - unsCents) + 50) / 100};
                    } else {
                        result = {names[m], (static_cast<float>(unsCents - note) + 50) / 100};
                    }
                    // we found our note so we can return right now
                    goto exit;
                }
                // iterate by whole note
                note += 100;
            }
        }
        exit:
        return result;
    }
}