//
// Created by lueroth on 24.03.2026.
//
#include "FindNote_n_Offset.h"

#include <string>
#include <array>
#include "YINPitchDetection.h"

#include "vstgui4/vstgui/lib/platform/win32/winstring.h"

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

    double FindNote_n_Offset::calcCents(double freq) const {
            return 1200 * log2(freq / refFreq_);
    }

    note_n_offset FindNote_n_Offset::findNote(const float *samples, size_t count) {
        // get frequency
        double freq = detection.detect(samples, count);

        if (freq <= 0.0f) return {"  ", 0.5f};

        double cents = calcCents(freq);
        double unsCents = cents;
        bool reverse {false};

        // detect reverse order and invert number for logarithmic equations
        if (unsCents < 0) {
            unsCents = -unsCents;
            reverse = true;
        }

        int note {};
        note_n_offset result {};

        // there is 9 octaves, but because of reference note this number doesn't make sense
        // does not need fixing, the number is high enough
        for (int n = 0; n <= 9; n++) {
            bool flag {false};

            // 12 notes. 13th note in the array is purely for reverse order.
            for (int m = 0; m <= 11; m++) {

                if (unsCents <= note) {

                    // if cents is negative we've to count in reverse note order
                    if (reverse) {
                        // if difference is higher than 50 we missed by one note
                        if (note - unsCents > 50) {
                            if (m == 0) {
                                // 13 - 0 is out of range, and because of reverse order 13 actually means 1
                                result = {names[1], (float(note - unsCents - 100) + 50) / 100};
                            } else {
                                // m is not 0 so no out of range possible
                                result = {names[13 - m], (float(note - unsCents - 100) + 50) / 100};
                            }
                        } else {
                            // we didn't miss by a note so there is no offset for names
                            result = {names[12 - m], (float(note - unsCents) + 50) / 100};
                        }
                    } else {
                        if (note - unsCents > 50) {
                            if (m == 0) {
                                result = {names[11], (float(note - unsCents - 100) + 50) / 100};
                            } else {
                                result = {names[m - 1], (float(100 - note + unsCents) + 50) / 100};
                            }
                        } else {
                            result = {names[m], (float(note - unsCents) + 50) / 100};
                        }
                    }
                    // we found our note so no calculations have to be done further
                    flag = true;
                    break;
                }
                // iterate by whole note
                note += 100;
            }
            if (flag) break;
        }

        return result;
    }
}