#include "stdafx.h"
#include <windows.h>
#include "..\Gauss.h"
#include "..\simpleSafeArray.h"
#include "..\simpleVec2.h"

// convert SimpleDVec2 to scalar which is used as sampling density we want Metropolis to converge to
inline double toScalar(const SimpleDVec2 &v) { return v[0] + v[1]; }

// piece-wise linear function so that integral could be computed exactly in analytic fashion
// the function is made 2D on purpose - so that we wouldn't be able to use it as sampling density
// directly (we apply toScalar() to get sampling density)
struct MyFunction
{
    MyFunction()
    {
        m_xMin = -10;
        m_xMax =  10;
        // initialize using two different gaussians (one for each output dimension)
        Gaussian gauss1(1, -5, 0.5), gauss2(2, 5, 0.5);
        for (NvU32 u = 0; u < ARRAY_ELEMENT_COUNT(m_pValues); ++u)
        {
            double curX = indexToCoordinate(u);
            m_pValues[u][0] = gauss1.evalAt(curX);
            m_pValues[u][1] = gauss2.evalAt(curX);
        }
    }

    // interval that function is defined upon
    double getXMin() const { return m_xMin; }
    double getXMax() const { return m_xMax; }

    // exact integral value to compare against
    SimpleDVec2 evalFullIntegral() const
    {
        SimpleDVec2 fSum = m_pValues[0] / 2;
        for (NvU32 u = 1; u < NINTERVALS; ++u)
        {
            fSum += m_pValues[u];
        }
        fSum += m_pValues[NINTERVALS] / 2;
        return fSum * (m_xMax - m_xMin) / NINTERVALS;
    }

    // evaluate value of function at specific coordinate (used for numeric integration)
    SimpleDVec2 evalAt(double x) const
    {
        if (x < m_xMin || x > m_xMax)
            return SimpleDVec2(0, 0);
        double fCoord = coordinateToIndex(x);
        NvU32 uCoord = (NvU32)fCoord;
        double fInterp = fCoord - uCoord;
        return m_pValues[uCoord] * (1 - fInterp) + m_pValues[uCoord + 1] * fInterp;
    }

private:
    inline double indexToCoordinate(NvU32 u) const
    {
        double f = u / (double)NINTERVALS;
        return m_xMin * (1 - f) + m_xMax * f;
    }
    inline double coordinateToIndex(double f) const
    {
        return (f - m_xMin) / (m_xMax - m_xMin) * NINTERVALS;
    }
    static const NvU32 NVALUES = 1024 * 20 + 1;
    static const NvU32 NINTERVALS = NVALUES - 1;
    double m_xMin, m_xMax;
    SimpleSafeArray<SimpleDVec2, NVALUES + 2> m_pValues;
};

// this demonstrates evaluation of integral from MyFunction using Metropolis approach, without
// need for burn-in samples and without bias
// the idea comes from "ROBUST MONTE CARLO METHODS FOR LIGHT TRANSPORT SIMULATION" - the Ph.D.
// thesis by Eric Veach (section 11.3.1 - Eliminating start-up bias)
int main()
{
    // initialize generator differently each time - helps with testing for bias
    RNGUniform rng(GetTickCount());
    MyFunction function;

    auto fullIntegral = function.evalFullIntegral();
    printf("Exact integral is: (%e, %e)\n", fullIntegral[0], fullIntegral[1]);

    // can be any number from 0 to infinity. when this is zero, you just get uniform samples without Metropolis mutations
    const NvU32 nMutationsPerPath = 10;

    const double fJumpMultipler = 1; // affects magnitude of Metropolis jump on every step
    // generator of normally distributed numbers for Metropolis jumps
    StandardGaussDistribution gauss;

    // for simplicity, our paths have just one point in them, but this is a test for more general case so we still call them paths
    SimpleDVec2 fGlobalSumOfValues(0, 0);
    double fGlobalSumOfWeights = 0;
    for (NvU64 nSamples = 0, nSamplesToPrint = 1; ; )
    {
        // first sample is uniformly distributed
        double fX = rng.generateBetween(function.getXMin(), function.getXMax());
        auto fValue = function.evalAt(fX);
        double fScalar = toScalar(fValue);
        double fCurrentWeight = 1; // weight of the first (uniformly distributed) sample

        // remember the first scalar because it will be used for weighting mutated paths in unbiased way
        double fScalar0 = fScalar;

        for (NvU32 uMutation = 0; ; )
        {
            fGlobalSumOfValues += fValue * fCurrentWeight;
            fGlobalSumOfWeights += fCurrentWeight;

            // print out the result at 1, 2, 4, 8, 16, etc. samples
            if (++nSamples >= nSamplesToPrint)
            {
                nSamplesToPrint *= 2;
                auto fGlobalValue = fGlobalSumOfValues / (double)nSamples;
                auto fResult = fGlobalValue * (function.getXMax() - function.getXMin());

                DWORD bit = 0;
                BitScanReverse64(&bit, nSamples);
                printf("%d: (%e, %e)\n", bit, fResult[0], fResult[1]);
            }

            // mutate the existing point according to Metropolis algorithm specified number of times
            if (++uMutation >= nMutationsPerPath)
                break;

            double fNextX = fX + gauss.generate(rng) * fJumpMultipler;
            auto fNextValue = function.evalAt(fNextX);
            double fNextScalar = toScalar(fNextValue);

            double fAcceptanceProbability = fNextScalar / fScalar;
            if (fAcceptanceProbability >= 1 || rng.generate01() < fAcceptanceProbability)
            {
                fX = fNextX;
                fValue = fNextValue;
                fScalar = fNextScalar;
                // normally Metropolis would have weight equal to (1. / fScalar), but fScalar0 is required to
                // make weight of Metropolis samples consistent with the weight of the very first sample which
                // was uniformly distributed
                fCurrentWeight = fScalar0 / fScalar;
            }
        }
    }
 
    return 0;
}

