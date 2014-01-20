// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_HOUSE_LEVEL_H_INCLUDE_
#define _CAESARIA_HOUSE_LEVEL_H_INCLUDE_

#include "good/good.hpp"
#include "core/scopedptr.hpp"
#include "core/smartptr.hpp"
#include "vfs/path.hpp"
#include "predefinitions.hpp"
#include "service.hpp"

class HouseLevelSpec
{
  friend class HouseSpecHelper;

public:
  int getLevel() const;
  int getMaxHabitantsByTile() const;
  int getTaxRate() const;
  int getProsperity() const;
  int getCrime() const;

  // return the house type "small casa, luxury villa, ..."
  const std::string& getLevelName() const;
  const std::string& getInternalName() const;

  int getRequiredGoodLevel(Good::Type type) const;

  // returns True if patrician villa
  bool isPatrician() const;

  bool checkHouse( HousePtr house, std::string* retMissing = 0);
  unsigned int getServiceConsumptionInterval() const;
  unsigned int getFoodConsumptionInterval() const;
  unsigned int getGoodConsumptionInterval() const;

  int findLowLevelHouseNearby( HousePtr house, std::string &oMissingRequirement );

  HouseLevelSpec next() const;

  int computeDesirabilityLevel(HousePtr house, std::string &oMissingRequirement) const;
  int computeEntertainmentLevel(HousePtr house) const;
  int computeEducationLevel(HousePtr house, std::string &oMissingRequirement);
  int computeHealthLevel(HousePtr house, std::string &oMissingRequirement);
  int computeReligionLevel(HousePtr house);
  int computeWaterLevel(HousePtr house, std::string &oMissingRequirement);
  int computeFoodLevel(HousePtr house);
  int computeMonthlyGoodConsumption(HousePtr house, const Good::Type goodType, bool real) const;
  int computeMonthlyFoodConsumption( HousePtr house ) const;

  float evaluateServiceNeed(HousePtr house, const Service::Type service);
  float evaluateEntertainmentNeed(HousePtr house, const Service::Type service);
  float evaluateEducationNeed(HousePtr house, const Service::Type service);
  float evaluateHealthNeed(HousePtr house, const Service::Type service);
  float evaluateReligionNeed(HousePtr house, const Service::Type service);
  // float evaluateFoodNeed(House &house, const ServiceType service);


  int getMinEntertainmentLevel() const;
  int getMinEducationLevel() const;
//    int getMinHealthLevel();
  int getMinReligionLevel() const;
//    int getMinWaterLevel();
  int getMinFoodLevel() const;
  ~HouseLevelSpec();
  HouseLevelSpec();
  HouseLevelSpec( const HouseLevelSpec& other );
  HouseLevelSpec& operator=(const HouseLevelSpec& other );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class HouseSpecHelper
{
public:
  static HouseSpecHelper& getInstance();

  HouseLevelSpec getHouseLevelSpec(const int houseLevel);
  int getHouseLevel(const int houseId);
  int getHouseLevel( const std::string& name );
  void initialize( const vfs::Path& filename );

  ~HouseSpecHelper();
private:
  HouseSpecHelper();
  
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //_CAESARIA_HOUSE_LEVEL_H_INCLUDE_
