#ifndef MIM_IMAGES_H
#define MIM_IMAGES_H

#include <cmath>
#include "color.h"
#include "coordinate.h"
#include "functional.h"

using Fraction = double;

template<typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;
using Image = Base_image<Color>;
using Blend = Base_image<Fraction>;

namespace Detail{
  inline auto force_polar(const Point p){
    return p.is_polar ? p : to_polar(p);
  }
  inline auto force_cartesian(const Point p){
    return p.is_polar ? from_polar(p) : p;
  }
}

template <typename T>
Base_image<T> constant(T t){
  return [=]([[maybe_unused]]const Point p){ return t; };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi){
  return compose(
    Detail::force_polar,
    [=](const Point p){
      return image(Point(p.first, p.second - phi, true));
    }
  );
}

template <typename T>
Base_image<T> translate(Base_image<T> image, Vector v){
	return compose(
    Detail::force_cartesian,
    [=](const Point p){
      return image(Point(p.first - v.first, p.second - v.second, false));
    }
  );
}

template <typename T>
Base_image<T> scale(Base_image<T> image, double s){
  return compose(
    Detail::force_cartesian,
    [=](const Point p){
      return image(Point(p.first / s, p.second / s, false));
    }
  );
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer){
  return [=](const Point p){
    return (distance(Detail::force_cartesian(p),
      Detail::force_cartesian(q)) <= r) ? inner : outer;
  };
}

template <typename T>
Base_image<T> checker(double d, T this_way, T that_way){
  return compose(
    Detail::force_cartesian,
    [=](const Point p){
      return static_cast<int>(std::floor(p.first / d) +
        std::floor(p.second / d)) % 2 ? that_way : this_way;
    }
  );
}

template <typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way){
	return compose(
    Detail::force_polar,
    [=](const Point p){
      return Point(p.first, p.second * n * d / (2 * M_PI), false);
    },
    checker(d, this_way, that_way)
  );
}

template <typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way){
	return [=](const Point p){
		return static_cast<int>(distance(Detail::force_cartesian(p),
      Detail::force_cartesian(q)) / d) % 2 ? that_way : this_way;
	};
}

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way){
	return compose(
    Detail::force_cartesian,
    [=](const Point p){
		  return abs(p.first) <= d / 2 ? this_way : that_way;
	  }
  );
}

inline Image cond(Region region, Image this_way, Image that_way){
	return [=](const Point p){
		return region(p) ? this_way(p) : that_way(p);
	};
}

inline Image lerp(Blend blend, Image this_way, Image that_way){
  return lift(
    [=](Color first, Color second, Fraction f){
      return first.weighted_mean(second, f);
    },
    this_way,
    that_way,
    blend
  );
}

inline Image darken(Image image, Blend blend){
	return [=](const Point p){
    return image(p).weighted_mean(Colors::black, blend(p));
	};
}

inline Image lighten(Image image, Blend blend){
	return [=](const Point p){
		return image(p).weighted_mean(Colors::white, blend(p));
	};
}

#endif