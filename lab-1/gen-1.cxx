#include <cassert>
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

static std::random_device rd;

static constexpr double frac(const unsigned n, const unsigned d) {
	return static_cast<double>(n)/static_cast<double>(d);
}

static constexpr unsigned max_n_doors = 20;
static constexpr unsigned n_rounds = 3200;

static unsigned n_wins[1 + max_n_doors][1 + max_n_doors - 2][2];

int main() {
	// we should be able to pick a door  =>  n_doors >= 1
	// the host should be able to pick another door  =>  n_doors >= 2
	// we should be able to switch to yet another door  =>  n_doors >= 3
	for (unsigned n_doors = 3; n_doors <= max_n_doors; ++n_doors) {
		std::cout << "nd = " << n_doors << std::endl;

		std::uniform_int_distribution<unsigned> uid(0, n_doors - 1);

		std::vector<char> doors(n_doors);

		// if we pick a one of the goats, there should be another goat for the host to pick  =>  n_goats >= 2  =>  n_doors - n_cars >= 2  =>  n_cars <= n_doors - 2, n_doors >= n_cars + 2
		for (unsigned n_cars = 0; n_cars <= n_doors - 2; ++n_cars) {
			//std::cout << "\tnc = " << n_cars << std::endl;

			//const unsigned n_goats = n_doors - n_cars;

			std::fill(doors.begin(), doors.begin() + n_cars, 'c');
			std::fill(doors.begin() + n_cars, doors.end(), 'g');

			unsigned n_wins_wo_change = 0;
			unsigned n_wins_w_change = 0;
			for (unsigned r = 0; r < n_rounds; ++r) {
				std::shuffle(doors.begin(), doors.end(), rd);

				const unsigned p = uid(rd);

				unsigned h;
				do {
					h = uid(rd);
				} while (h == p || doors[h] == 'c');

				#if 0
				const unsigned hi = std::uniform_int_distribution<unsigned>(0, n_goats - 1)(rd);
				unsigned h = 0;
				for (unsigned j = 0; j < hi; ++j) {
					do {
						h++
					} (h == p || doors[h] == 'c');
				}
				#endif

				const unsigned np_wo_change = p;

				unsigned np_w_change;
				do {
					np_w_change = uid(rd);
				} while (np_w_change == p || np_w_change == h);

				if (doors[np_w_change] == 'c') {
					n_wins_w_change++;
				}
				if (doors[np_wo_change] == 'c') {
					n_wins_wo_change++;
				}
			}
			n_wins[n_doors][n_cars][0] = n_wins_wo_change;
			n_wins[n_doors][n_cars][1] = n_wins_w_change;
		}
	}

	#if 0
	f.write(reinterpret_cast<const char *>(&n_doors), sizeof(n_doors));
	f.write(reinterpret_cast<const char *>(&n_cars), sizeof(n_cars));
	f.write(reinterpret_cast<const char *>(&n_wins_wo_change), sizeof(n_wins_wo_change));
	f.write(reinterpret_cast<const char *>(&n_wins_w_change), sizeof(n_wins_w_change));
	#endif

	{
		std::ofstream f("data.txt");
		std::ofstream g("data3.txt");
		for (unsigned n_doors = 3; n_doors <= max_n_doors; ++n_doors) {
			for (unsigned n_cars = 0; n_cars <= n_doors - 2; ++n_cars) {
				f << n_doors << "\t" << n_cars << "\t" << n_wins[n_doors][n_cars][0] << "\t" << n_wins[n_doors][n_cars][1] << "\n";
				g << n_doors << "\t" << n_cars << "\t" << frac(n_cars, n_doors) << "\t" << frac(n_cars*(n_doors-1), n_doors*(n_doors-2)) << "\n";
			}
			f << "\n";
			g << "\n";
		}
	}

	{
		std::ofstream g("data2.txt");
		for (unsigned n_cars = 0; n_cars <= max_n_doors - 2; ++n_cars) {
			for (unsigned n_doors = std::max(3u, n_cars + 2); n_doors <= max_n_doors; ++n_doors) {
				g << n_doors << "\t" << n_cars << "\t" << n_wins[n_doors][n_cars][0] << "\t" << n_wins[n_doors][n_cars][1] << "\n";
			}
			g << "\n";
		}
	}

	#if 0
	std::cout << n_goats << " goats + " << n_cars << " cars = " << n_doors << " doors\n";
	std::cout << "theoretical loos prob. = " << n_cars << "/" << n_doors << " ~= " << frac(n_cars, n_doors)*100 << "% ~= 1/" << frac(n_doors, n_cars) << "\n";
	std::cout << "w: " << n_wins_w_change << "/" << n_rounds << " ~= " << frac(n_wins_w_change, n_rounds)*100 << "% ~= 1/" << frac(n_rounds, n_wins_w_change) << "\n";
	std::cout << "l: " << n_losses << "/" << n_rounds << " ~= " << frac(n_losses, n_rounds)*100 << "% ~= 1/" << frac(n_rounds, n_losses) << "\n";
	#endif

	return 0;
}
