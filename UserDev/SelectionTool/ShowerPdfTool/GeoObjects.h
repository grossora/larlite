/**
 * \file GeoObjects.h
 *
 * \ingroup GeoAlgo
 * 
 * \brief Class def header for a class GeoObjects
 *
 * @author kazuhiro
 */

/** \addtogroup GeoAlgo

    @{*/
#ifndef LARLITE_GEOOBJECTS_H
#define LARLITE_GEOOBJECTS_H

#include "GeoAlgoException.h"
#include <TVector3.h>
#include <TLorentzVector.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
namespace geoalgo {

  // Forward declaration (don't worry)
  class Trajectory;
  class PointToLineDist;

  /**
     \class Point
     This class represents an n-dimensional point
  */
  class Point : public std::vector<double> {
    friend class Trajectory;
    friend class PointToLineDist;

  public:
    /// Default ctor
    Point() : std::vector<double>()
    {}

    /// Ctor to instantiate with invalid value
    Point(size_t n) : std::vector<double>(n,std::numeric_limits<double>::max())
    {}

    /// Default ctor w/ a bare std::vector<double>
    Point(const std::vector<double> &obj) : std::vector<double>(obj)
    {}

    /// ctor w/ x & y
    Point(const double x, const double y) : Point(2)
    { (*this)[0] = x; (*this)[1] = y; }

    /// ctor w/ x, y & z
    Point(const double x, const double y, const double z) : Point(3)
    { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }

    /// ctor w/ TVector3
    Point(const TVector3 &pt) : Point(3)
    { (*this)[0] = pt[0]; (*this)[1] = pt[1]; (*this)[2] = pt[2]; }

    /// ctor w/ TLorentzVector
    Point(const TLorentzVector &pt) : Point(3)
    { (*this)[0] = pt[0]; (*this)[1] = pt[1]; (*this)[2] = pt[2]; }
    
    /// Default dtor
    virtual ~Point(){}

    /// Compute squared distance between 2 points with dimension check (no sqrt particularly useful for speed)
    double SquaredDistance(const Point& obj) const {     
      compat(obj);
      return _SquaredDistance_(obj);      
    }

    /// Compute squared distance from the origin to the point
    double SquaredDistance() const {
      double res=0;
      for(auto const &v : (*this)) res += v*v;
      return res;
    }

    /// Compute distance between between 2 points with dimension check
    double Distance(const Point& obj) const
    { return sqrt(SquaredDistance(obj)); }

    /// Compute the length from the origin to this point
    double Distance() const
    { return sqrt(SquaredDistance()); }

    /// Compute a dot product of two points (each representing a vector from the origin)
    double DotProduct(const Point &obj) const {
      compat(obj);
      double res=0;
      for(size_t i=0; i<size(); ++i)
	res += (*this)[i] * obj[i];
      return res;
    }
      
    /// Dimensional check for a compatibility
    void compat(const Point& obj) const {
      if(size() != obj.size()) {
	std::ostringstream msg;
	msg << "<<" << __FUNCTION__ << ">>" 
	    << " size mismatch: "
	    << size() << " != " << obj.size()
	    << std::endl;
	throw GeoAlgoException(msg.str());
      }
    }

  public:

    Point& operator+=(const Point& rhs) { 
      compat(rhs); 
      for(size_t i=0; i<size(); ++i) (*this)[i] += rhs[i]; 
      return *this;
    }

    Point& operator-=(const Point& rhs) {
      compat(rhs);
      for(size_t i=0; i<size(); ++i) (*this)[i] -= rhs[i];
      return *this;
    }

    Point& operator*=(const double rhs) {
      for(auto &v : *this) v *= rhs;
      return *this;
    }

    Point& operator/=(const double rhs) {
      for(auto &v : *this) v /= rhs;
      return *this;
    }

    inline Point operator+(const Point& rhs) const
    { 
      Point res((*this));
      res+=rhs;
      return res;
    }

    inline Point operator-(const Point& rhs) const
    { 
      Point res((*this));
      res-=rhs;
      return res;
    }

    inline double operator*(const Point& rhs) const
    {
      compat(rhs);
      double res=0;
      for(size_t i=0; i<size(); ++i) res += (*this)[i] * rhs[i];
      return res;
    }

    inline Point operator*(const double& rhs) const
    {
      Point res((*this));
      for(auto& v : res) v *= rhs;
      return res;
    }

    inline Point operator/(const double& rhs) const
    {
      Point res((*this));
      for(auto& v : res) v /= rhs;
      return res;
    }

    /// Streamer
    #ifndef __CINT__
    friend std::ostream& operator << (std::ostream &o, ::geoalgo::Point const& a)
    { o << "Point ("; for(auto const& v : a) o << v << " ";
      o << ")";
      return o;
    }
    #endif

  protected:

    /// Compute a squared distance w/o dimensional check. Available to friend classes
    double _SquaredDistance_(const Point& obj) const {
      double dist2=0;
      for(size_t i=0; i<size(); ++i) dist2 += pow(obj[i] - (*this)[i],2);
      return dist2;
    }

    /// Compute a distance w/o dimensional check. Available to friend classes
    double _Distance_(const Point& obj) const
    { return sqrt(_SquaredDistance_(obj)); }
    
  };

  /**
     \class Trajectory
     This class represents a trajectory which is an ordered list of Point.
     It is a friend class w/ geoalgo::Point hence it has an access to protected functions that avoids
     dimensionality sanity checks for speed.
   */
  class Trajectory : public std::vector<geoalgo::Point> {

  public:

    /// Default ctor
    Trajectory() : std::vector<geoalgo::Point>()
    {}
    
    /// Ctor to specify # points and dimension of each point
    Trajectory(size_t npoints, size_t ndimension) : std::vector<geoalgo::Point>(npoints, Point(ndimension))
    {}

    /// Ctor using a vector of mere vector point expression
    Trajectory(const std::vector<std::vector<double> > &obj)
    {
      this->reserve(obj.size());
      for(auto const& p : obj) this->push_back(Point(p));
    }

    /// Ctor using a vector of point
    Trajectory(const std::vector<geoalgo::Point> &obj)
    {
      this->reserve(obj.size());
      for(auto const& p : obj) this->push_back(p);
    }

    /// Default dtor
    virtual ~Trajectory(){}

    /// Returns the cumulative distance along all trajectory points
    double Length() const {

      if(size()<2) return 0;

      double length = 0;
      for(size_t i=0; i<size()-1; ++i) 

	length += (*this)[i]._Distance_((*this)[i+1]);
      
      return length;
    }

    /// push_back overrie w/ dimensionality check 
    void push_back(const Point& obj) {
      compat(obj); std::vector<geoalgo::Point>::push_back(obj);
    }

    /// Dimensionality check function w/ Trajectory
    void compat(const Point& obj) const {

      if(!size()) return;
      if( (*(this->begin())).size() != obj.size() ) {

	std::ostringstream msg;
	msg << "<<" << __FUNCTION__ << ">>"
	    << " size mismatch: "
	    << (*(this->begin())).size() << " != " << obj.size() << std::endl;
	throw GeoAlgoException(msg.str());
      }
    }

    /// Dimensionality check function w/ Point
    void compat(const Trajectory &obj) const {

      if(!size() || !(obj.size())) return;

      if( (*(this->begin())).size() != (*obj.begin()).size() ) {

	std::ostringstream msg;
	msg << "<<" << __FUNCTION__ << ">>"
	    << " size mismatch: "
	    << (*(this->begin())).size() << " != " << (*obj.begin()).size() << std::endl;
	throw GeoAlgoException(msg.str());

      }
    }

  public:

    /// Streamer
    #ifndef __CINT__
    friend std::ostream& operator << (std::ostream &o, Trajectory const& a)
    { o << "Trajectory with " << a.size() << " points " << std::endl;
      for(auto const& p : a )
	o << " " << p << std::endl;
      return o;
    }
    #endif
  };

}

#endif
/** @} */ // end of doxygen group 

