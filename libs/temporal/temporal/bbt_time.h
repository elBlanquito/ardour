/*
  Copyright (C) 2002-2010 Paul Davis

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __timecode_bbt_time_h__
#define __timecode_bbt_time_h__

#include <ostream>
#include <istream>
#include <stdint.h>
#include <iomanip>
#include <exception>
#include <cmath>
#include <limits>

#include "temporal/visibility.h"
#include "temporal/types.h"

namespace Temporal {

struct BBT_Offset;

/** Bar, Beat, Tick Time (i.e. Tempo-Based Time) */
struct LIBTEMPORAL_API BBT_Time
{
	/* note that it is illegal for BBT_Time to have bars==0 or
	 * beats==0. The "neutral" or "default" value is 1|1|0
	 */

	int32_t bars;
	int32_t beats;
	int32_t ticks;

	struct IllegalBBTTimeException : public std::exception {
		virtual const char* what() const throw() { return "illegal BBT time (bars or beats were zero)"; }
	};

	BBT_Time () : bars (1), beats (1), ticks (0) {}
	BBT_Time (int32_t ba, uint32_t be, uint32_t t) : bars (ba), beats (be), ticks (t) { if (!bars || !beats) { throw IllegalBBTTimeException(); } }

	bool operator< (const BBT_Time& other) const {
		return bars < other.bars ||
			(bars == other.bars && beats < other.beats) ||
			(bars == other.bars && beats == other.beats && ticks < other.ticks);
	}

	bool operator<= (const BBT_Time& other) const {
		return bars < other.bars ||
			(bars <= other.bars && beats <= other.beats) ||
			(bars <= other.bars && beats <= other.beats && ticks <= other.ticks);
	}

	bool operator> (const BBT_Time& other) const {
		return bars > other.bars ||
			(bars == other.bars && beats > other.beats) ||
			(bars == other.bars && beats == other.beats && ticks > other.ticks);
	}

	bool operator>= (const BBT_Time& other) const {
		return bars > other.bars ||
			(bars >= other.bars && beats >= other.beats) ||
			(bars >= other.bars && beats >= other.beats && ticks >= other.ticks);
	}

	bool operator== (const BBT_Time& other) const {
		return bars == other.bars && beats == other.beats && ticks == other.ticks;
	}

	bool operator!= (const BBT_Time& other) const {
		return bars != other.bars || beats != other.beats || ticks != other.ticks;
	}

	bool operator<  (const BBT_Offset& other) const;
	bool operator<= (const BBT_Offset& other) const;
	bool operator>  (const BBT_Offset& other) const;
	bool operator>= (const BBT_Offset& other) const;
	bool operator== (const BBT_Offset& other) const;
	bool operator!= (const BBT_Offset& other) const;

	/* it would be nice to provide +,-,* and / operators for BBT_Time but
	 * this math requires knowledge of the meter (time signature) used to
	 * define 1 bar, and so cannot be carried out with only two BBT_Time
	 * values.
	 */

	BBT_Time round_to_beat () const { return ticks >= (ticks_per_beat/2) ? BBT_Time (bars, beats+1, 0) : BBT_Time (bars, beats, 0); }
	BBT_Time round_down_to_beat () const { return BBT_Time (bars, beats, 0); }
	BBT_Time round_up_to_beat () const { return ticks ? BBT_Time (bars, beats+1, 0) : *this; }

	/* cannot implement round_to_bar() without knowing meter (time
	 * signature) information.
	 */

	void print_padded (std::ostream& o) {
		o << std::setfill ('0') << std::right
		  << std::setw (3) << bars << "|"
		  << std::setw (2) << beats << "|"
		  << std::setw (4) << ticks;
	}
};

struct LIBTEMPORAL_API BBT_Offset
{
	int32_t bars;
	int32_t beats;
	int32_t ticks;

	/* this is a variant for which bars==0 and/or beats==0 is legal. It
	 * represents an offset from a given BBT_Time and is used when doing
	 * add/subtract operations on a BBT_Time.
	 */

	BBT_Offset () : bars (0), beats (0), ticks (0) {}
	BBT_Offset (int32_t ba, uint32_t be, uint32_t t) : bars (ba), beats (be), ticks (t) {}
	BBT_Offset (BBT_Time const & bbt) : bars (bbt.bars), beats (bbt.beats), ticks (bbt.ticks) {}
	BBT_Offset (double beats);

	/* unlike BBT_Time, we can define +,-,* and / operators for BBT_Offset
	 * because there is no requirement that the result is "well-formed" or
	 * reflect the structure of a tempo map. It is just as valid to talk
	 * about an offset of 18 beats as an offset of 4 bars and 2 beats.
	 */

	BBT_Offset operator+(const BBT_Offset& other) const {
		return BBT_Offset (bars+other.bars, beats+other.beats, ticks+other.ticks);
	}

	BBT_Offset operator-() const {
		return BBT_Offset (-bars, -beats, -ticks);
	}

	BBT_Offset operator-(const BBT_Offset& other) const {
		return BBT_Offset (bars-other.bars, beats-other.beats, ticks-other.ticks);
	}

	BBT_Offset & operator+=(const BBT_Offset& other) {
		bars += other.bars;
		beats += other.beats;
		ticks += other.ticks;
		return *this;
	}

	BBT_Offset & operator-=(const BBT_Offset& other) {
		bars -= other.bars;
		beats -= other.beats;
		ticks -= other.ticks;
		return *this;
	}

	BBT_Offset & operator*=(int factor) {
		bars *= factor;
		beats *= factor;
		ticks *= factor;
		return *this;
	}

	BBT_Offset & operator*=(double factor) {
		bars = lrint (bars * factor);
		beats = lrint (beats * factor);
		ticks = lrint (ticks * factor);
		return *this;
	}

	BBT_Offset & operator/=(int factor) {
		bars /= factor;
		beats /= factor;
		ticks /= factor;
		return *this;
	}

	BBT_Offset & operator/=(double factor) {
		bars = lrint (bars / factor);
		beats = lrint (beats / factor);
		ticks = lrint (ticks / factor);
		return *this;
	}

	bool operator< (const BBT_Offset& other) const {
		return bars < other.bars ||
			(bars == other.bars && beats < other.beats) ||
			(bars == other.bars && beats == other.beats && ticks < other.ticks);
	}

	bool operator<= (const BBT_Offset& other) const {
		return bars < other.bars ||
			(bars <= other.bars && beats <= other.beats) ||
			(bars <= other.bars && beats <= other.beats && ticks <= other.ticks);
	}

	bool operator> (const BBT_Offset& other) const {
		return bars > other.bars ||
			(bars == other.bars && beats > other.beats) ||
			(bars == other.bars && beats == other.beats && ticks > other.ticks);
	}

	bool operator>= (const BBT_Offset& other) const {
		return bars > other.bars ||
			(bars >= other.bars && beats >= other.beats) ||
			(bars >= other.bars && beats >= other.beats && ticks >= other.ticks);
	}

	bool operator== (const BBT_Offset& other) const {
		return bars == other.bars && beats == other.beats && ticks == other.ticks;
	}

	bool operator!= (const BBT_Offset& other) const {
		return bars != other.bars || beats != other.beats || ticks != other.ticks;
	}
};

inline bool
BBT_Time::operator< (const BBT_Offset& other) const
{
	return bars < other.bars ||
	              (bars == other.bars && beats < other.beats) ||
		(bars == other.bars && beats == other.beats && ticks < other.ticks);
}

inline bool
BBT_Time::operator<= (const BBT_Offset& other) const
{
	return bars < other.bars ||
	              (bars <= other.bars && beats <= other.beats) ||
		(bars <= other.bars && beats <= other.beats && ticks <= other.ticks);
}

inline bool
BBT_Time::operator> (const BBT_Offset& other) const
{
	return bars > other.bars ||
		(bars == other.bars && beats > other.beats) ||
		(bars == other.bars && beats == other.beats && ticks > other.ticks);
}

inline bool
BBT_Time::operator>= (const BBT_Offset& other) const
{
	return bars > other.bars ||
		(bars >= other.bars && beats >= other.beats) ||
		(bars >= other.bars && beats >= other.beats && ticks >= other.ticks);
}

inline bool
BBT_Time::operator== (const BBT_Offset& other) const
{
	return bars == other.bars && beats == other.beats && ticks == other.ticks;
}

inline bool
BBT_Time::operator!= (const BBT_Offset& other) const
{
	return bars != other.bars || beats != other.beats || ticks != other.ticks;
}

} /* end of namespace Temporal */


/* Putting these into std:: seems wrong, but g++ is unable to find them
 * otherwise
 */

namespace std {

std::ostream& operator<< (std::ostream& o, Temporal::BBT_Time const & bbt);
std::ostream& operator<< (std::ostream& o, Temporal::BBT_Offset const & bbt);
std::istream& operator>> (std::istream& i, Temporal::BBT_Time& bbt);
std::istream& operator>> (std::istream& i, Temporal::BBT_Offset& bbt);

template<>
struct numeric_limits<Temporal::BBT_Time> {
	static Temporal::BBT_Time lowest() {
		return Temporal::BBT_Time(1, 1, 0);
	}
	static Temporal::BBT_Time min() {
		return Temporal::BBT_Time(1, 1, 0);
	}

	static Temporal::BBT_Time max() {
		return Temporal::BBT_Time(std::numeric_limits<int32_t>::max(),
		                          std::numeric_limits<int32_t>::max(),
		                          std::numeric_limits<int32_t>::max());
	}
};

template<>
struct numeric_limits<Temporal::BBT_Offset> {
	static Temporal::BBT_Offset lowest() {
		return Temporal::BBT_Time(0, 0, 0);
	}

	static Temporal::BBT_Offset min() {
		return Temporal::BBT_Time(0, 0, 0);
	}

	static Temporal::BBT_Time max() {
		return Temporal::BBT_Time(std::numeric_limits<int32_t>::max(),
		                          std::numeric_limits<int32_t>::max(),
		                          std::numeric_limits<int32_t>::max());
	}
};


} /* end of namespace std */

#endif /* __timecode_bbt_time_h__ */