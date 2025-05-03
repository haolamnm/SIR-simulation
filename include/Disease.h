#ifndef DISEASE_H
#define DISEASE_H

class Disease {
   private:
    float transmission_rate;
    float fatality_rate;
    int days_in_incubation;
    int days_with_symptoms;

   public:
    Disease(float transmission_rate, float fatality_rate, int days_in_incubation,
            int days_with_symptoms);

    float get_transmission_rate() const;
    float get_fatality_rate() const;
    int get_days_in_incubation() const;
    int get_days_with_symptoms() const;

   private:
};

#endif
