#ifndef ORGANISM_H
#define ORGANISM_H

#include <concepts>
#include <cstdint>
#include <tuple>
#include <optional>

template<typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism {
private:
	uint64_t vitality;
	species_t species;

public:
	constexpr uint64_t get_vitality() const {
		return vitality;
	}

	constexpr Organism(species_t const &species, uint64_t vitality) :
			vitality(vitality), species(species) {}

	constexpr const species_t &get_species() const {
		return species;
	}

	constexpr bool is_dead() const {
		return vitality == 0;
	}

	constexpr bool is_plant() const {
		return !can_eat_meat && !can_eat_plants;
	}

	constexpr void set_vitality(uint64_t vitality_) {
		vitality = vitality_;
	}
};

template<typename species_t>
using Carnivore = Organism<species_t, true, false>;

template<typename species_t>
using Omnivore = Organism<species_t, true, true>;

template<typename species_t>
using Herbivore = Organism<species_t, false, true>;

template<typename species_t>
using Plant = Organism<species_t, false, false>;


template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
requires (sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p)
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
		Organism<species_t, sp2_eats_m, sp2_eats_p>,
		std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(const Organism<species_t, sp1_eats_m, sp1_eats_p> &organism1,
		  const Organism<species_t, sp2_eats_m, sp2_eats_p> &organism2) {
	uint64_t vitality1 = organism1.get_vitality(), vitality2 = organism2.get_vitality();
	Organism<species_t, sp1_eats_m, sp1_eats_p> organism1_copy(organism1.get_species(), vitality1);
	Organism<species_t, sp2_eats_m, sp2_eats_p> organism2_copy(organism2.get_species(), vitality2);

	if (organism1.is_dead() || organism2.is_dead())
		return {organism1_copy, organism2_copy, {}};

	if (organism1.get_species() == organism2.get_species() &&
			sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p) {
		Organism<species_t, sp1_eats_m, sp1_eats_p>
		        offspring(organism1.get_species(), (vitality1 + vitality2) / 2);
		return {organism1_copy, organism2_copy, offspring};
	}

	if (sp1_eats_m && sp2_eats_m) {
		if (vitality1 < vitality2) {
			organism1_copy.set_vitality(0);
			organism2_copy.set_vitality(vitality2 + vitality1 / 2);
		}
		else if (vitality1 > vitality2) {
			organism1_copy.set_vitality(vitality1 + vitality2 / 2);
			organism2_copy.set_vitality(0);
		}
		else {
			organism1_copy.set_vitality(0);
			organism2_copy.set_vitality(0);
		}
		return {organism1_copy, organism2_copy, {}};
	}

	if (sp1_eats_p && organism2.is_plant()) {
		organism1_copy.set_vitality(vitality1 + vitality2);
		organism2_copy.set_vitality(0);
		return {organism1_copy, organism2_copy, {}};
	}

	if (organism1.is_plant() && sp2_eats_p) {
		organism1_copy.set_vitality(0);
		organism2_copy.set_vitality(vitality1 + vitality2);
		return {organism1_copy, organism2_copy, {}};
	}

	if (sp1_eats_m && !organism2.is_plant()) {
		if (vitality1 > vitality2) {
			organism1_copy.set_vitality(0);
			organism2_copy.set_vitality(vitality2 + vitality1 / 2);
		}
		return {organism1_copy, organism2_copy, {}};
	}

	if (!organism1.is_plant() && sp2_eats_m) {
		if (vitality2 > vitality1) {
			organism1_copy.set_vitality(vitality1 + vitality2 / 2);
			organism2_copy.set_vitality(0);
		}
		return {organism1_copy, organism2_copy, {}};
	}

	return {organism1_copy, organism2_copy, {}};
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
requires (sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p)
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
operator^(const Organism<species_t, sp1_eats_m, sp1_eats_p> &organism1,
		  const Organism<species_t, sp2_eats_m, sp2_eats_p> &organism2) {
	return std::get<0>(encounter(organism1, organism2));
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
	return (organism1 ^ ... ^ args);
}

#endif //ORGANISM_H