#ifndef DISEASE_H
#define DISEASE_H

#include <string>

/**
 * @class Disease
 * @brief Represents a disease with epidemiological parameters
 * */
class Disease {
   private:
    double transmission_rate = 0.0;  ///< Probability of disease transmission (0 to 1)
    double fatality_rate = 0.0;      ///< Probability of fatal outcome (0 to 1)
    int days_in_incubation = 0;      ///< Number of days in incubation period
    int days_with_symptoms = 0;      ///< Number of days with symptoms
    std::string name = "";           ///< Name of the disease

   public:
    Disease() = default;
    Disease(double transmission_rate, double fatality_rate, int days_in_incubation,
            int days_with_symptoms, const std::string &name = "");

    double get_transmission_rate() const;
    double get_fatality_rate() const;
    int get_days_in_incubation() const;
    int get_days_with_symptoms() const;
    std::string get_name() const;

    void set_transmission_rate(double rate);
    void set_fatality_rate(double rate);
    void set_days_in_incubation(int days);
    void set_days_with_symptoms(int days);
    void set_name(const std::string &name);

   private:
    void validate() const;
};

#endif
