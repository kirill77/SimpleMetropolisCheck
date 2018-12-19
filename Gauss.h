#pragma once

#include "RNGUniform.h"

// evaluates gaussian function with given total integral, mean, and variance
struct Gaussian
{
    Gaussian(double fTotalIntegral, double fMean, double fVariance)
    {
        m_fVarianceDoubled = 2 * fVariance;
        m_fMean = fMean;
        m_a = fTotalIntegral / (sqrt(fVariance * 2 * M_PI));
    }
    double evalAt(double x) const
    {
        return m_a * exp(-sqr((m_fMean - x)) / m_fVarianceDoubled);
    }
private:
    double m_a, m_fMean, m_fVarianceDoubled;
};

// generates samples with mean = 0, and variance = 1
struct StandardGaussDistribution
{
    StandardGaussDistribution() : m_nAvailable(0) { }

    double generate(RNGUniform &rng)
    {
        if (m_nAvailable == 0)
        {
            // basic box-mueller
            double u1 = sqrt(-2 * log(rng.generate01()));
            double u2 = 2 * M_PI * rng.generate01();
            m_fSamples[0] = u1 * sin(u2);
            m_fSamples[1] = u1 * cos(u2);
            m_nAvailable = 2;
        }
        return m_fSamples[--m_nAvailable];
    }

private:
   NvU32 m_nAvailable;
   double m_fSamples[2];
};