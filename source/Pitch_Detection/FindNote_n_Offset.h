//
// Created by lueroth on 24.03.2026.
//

#ifndef GENTEST_FINDNOTE_N_OFFSET_H
#define GENTEST_FINDNOTE_N_OFFSET_H

#include <string>
#include "YINPitchDetection.h"

namespace fno {
    struct note_n_offset {
        std::string note;
        float offset;
    };

    class FindNote_n_Offset {
    private:
        double refFreq_ {440.0};
        yin::YINPitchDetection detection{4096, 44100.0f};

    public:

        void setSampleRate(double sampleRate);
        FindNote_n_Offset(float refFreq);
        void setRefFreq(double refFreq);
        [[nodiscard]] double getRefFreq() const;

        note_n_offset findNote(const float *samples, size_t count);
    };
}

#endif //GENTEST_FINDNOTE_N_OFFSET_H