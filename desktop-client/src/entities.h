//
// Created by Mixerou on 18.12.2024.
//

#ifndef DESKTOP_CLIENT_ENTITIES_H
#define DESKTOP_CLIENT_ENTITIES_H

namespace entities {
typedef int64_t user_id_t;
typedef int64_t hotel_id_t;
typedef int64_t guest_id_t;
typedef int64_t room_id_t;
typedef int64_t booking_id_t;

// Copied from some gist
enum class Country : int16_t {
  kAfghanistan = 0,
  kAlbania,
  kAlgeria,
  kAndorra,
  kAngola,
  kAntiguaAndDeps,
  kArgentina,
  kArmenia,
  kAustralia,
  kAustria,
  kAzerbaijan,
  kBahamas,
  kBahrain,
  kBangladesh,
  kBarbados,
  kBelarus,
  kBelgium,
  kBelize,
  kBenin,
  kBhutan,
  kBolivia,
  kBosniaAndHerzegovina,
  kBotswana,
  kBrazil,
  kBrunei,
  kBulgaria,
  kBurkina,
  kBurundi,
  kCambodia,
  kCameroon,
  kCanada,
  kCapeVerde,
  kCentralAfricanRepublic,
  kChad,
  kChile,
  kChina,
  kColombia,
  kComoros,
  kCongo,
  kCongoDemocraticRepublic,
  kCostaRica,
  kCroatia,
  kCuba,
  kCyprus,
  kCzechRepublic,
  kDenmark,
  kDjibouti,
  kDominica,
  kDominicanRepublic,
  kEastTimor,
  kEcuador,
  kEgypt,
  kElSalvador,
  kEquatorialGuinea,
  kEritrea,
  kEstonia,
  kEthiopia,
  kFiji,
  kFinland,
  kFrance,
  kGabon,
  kGambia,
  kGeorgia,
  kGermany,
  kGhana,
  kGreece,
  kGrenada,
  kGuatemala,
  kGuinea,
  kGuineaBissau,
  kGuyana,
  kHaiti,
  kHonduras,
  kHungary,
  kIceland,
  kIndia,
  kIndonesia,
  kIran,
  kIraq,
  kIrelandRepublic,
  kIsrael,
  kItaly,
  kIvoryCoast,
  kJamaica,
  kJapan,
  kJordan,
  kKazakhstan,
  kKenya,
  kKiribati,
  kKoreaNorth,
  kKoreaSouth,
  kKosovo,
  kKuwait,
  kKyrgyzstan,
  kLaos,
  kLatvia,
  kLebanon,
  kLesotho,
  kLiberia,
  kLibya,
  kLiechtenstein,
  kLithuania,
  kLuxembourg,
  kMacedonia,
  kMadagascar,
  kMalawi,
  kMalaysia,
  kMaldives,
  kMali,
  kMalta,
  kMarshallIslands,
  kMauritania,
  kMauritius,
  kMexico,
  kMicronesia,
  kMoldova,
  kMonaco,
  kMongolia,
  kMontenegro,
  kMorocco,
  kMozambique,
  kMyanmarBurma,
  kNamibia,
  kNauru,
  kNepal,
  kNetherlands,
  kNewZealand,
  kNicaragua,
  kNiger,
  kNigeria,
  kNorway,
  kOman,
  kPakistan,
  kPalau,
  kPanama,
  kPapuaNewGuinea,
  kParaguay,
  kPeru,
  kPhilippines,
  kPoland,
  kPortugal,
  kQatar,
  kRomania,
  kRussianFederation,
  kRwanda,
  kStKittsAndNevis,
  kStLucia,
  kSaintVincentAndTheGrenadines,
  kSamoa,
  kSanMarino,
  kSaoTomeAndPrincipe,
  kSaudiArabia,
  kSenegal,
  kSerbia,
  kSeychelles,
  kSierraLeone,
  kSingapore,
  kSlovakia,
  kSlovenia,
  kSolomonIslands,
  kSomalia,
  kSouthAfrica,
  kSouthSudan,
  kSpain,
  kSriLanka,
  kSudan,
  kSuriname,
  kSwaziland,
  kSweden,
  kSwitzerland,
  kSyria,
  kTaiwan,
  kTajikistan,
  kTanzania,
  kThailand,
  kTogo,
  kTonga,
  kTrinidadAndTobago,
  kTunisia,
  kTurkey,
  kTurkmenistan,
  kTuvalu,
  kUganda,
  kUkraine,
  kUnitedArabEmirates,
  kUnitedKingdom,
  kUnitedStates,
  kUruguay,
  kUzbekistan,
  kVanuatu,
  kVaticanCity,
  kVenezuela,
  kVietnam,
  kYemen,
  kZambia,
  kZimbabwe,
};

constexpr auto kAllCountries = std::array{
    std::pair{Country::kAfghanistan, "Afghanistan"},
    std::pair{Country::kAlbania, "Albania"},
    std::pair{Country::kAlgeria, "Algeria"},
    std::pair{Country::kAndorra, "Andorra"},
    std::pair{Country::kAngola, "Angola"},
    std::pair{Country::kAntiguaAndDeps, "Antigua & Deps"},
    std::pair{Country::kArgentina, "Argentina"},
    std::pair{Country::kArmenia, "Armenia"},
    std::pair{Country::kAustralia, "Australia"},
    std::pair{Country::kAustria, "Austria"},
    std::pair{Country::kAzerbaijan, "Azerbaijan"},
    std::pair{Country::kBahamas, "Bahamas"},
    std::pair{Country::kBahrain, "Bahrain"},
    std::pair{Country::kBangladesh, "Bangladesh"},
    std::pair{Country::kBarbados, "Barbados"},
    std::pair{Country::kBelarus, "Belarus"},
    std::pair{Country::kBelgium, "Belgium"},
    std::pair{Country::kBelize, "Belize"},
    std::pair{Country::kBenin, "Benin"},
    std::pair{Country::kBhutan, "Bhutan"},
    std::pair{Country::kBolivia, "Bolivia"},
    std::pair{Country::kBosniaAndHerzegovina, "Bosnia Herzegovina"},
    std::pair{Country::kBotswana, "Botswana"},
    std::pair{Country::kBrazil, "Brazil"},
    std::pair{Country::kBrunei, "Brunei"},
    std::pair{Country::kBulgaria, "Bulgaria"},
    std::pair{Country::kBurkina, "Burkina"},
    std::pair{Country::kBurundi, "Burundi"},
    std::pair{Country::kCambodia, "Cambodia"},
    std::pair{Country::kCameroon, "Cameroon"},
    std::pair{Country::kCanada, "Canada"},
    std::pair{Country::kCapeVerde, "Cape Verde"},
    std::pair{Country::kCentralAfricanRepublic, "Central African Rep"},
    std::pair{Country::kChad, "Chad"},
    std::pair{Country::kChile, "Chile"},
    std::pair{Country::kChina, "China"},
    std::pair{Country::kColombia, "Colombia"},
    std::pair{Country::kComoros, "Comoros"},
    std::pair{Country::kCongo, "Congo"},
    std::pair{Country::kCongoDemocraticRepublic,
              "Democratic Republic of the Congo"},
    std::pair{Country::kCostaRica, "Costa Rica"},
    std::pair{Country::kCroatia, "Croatia"},
    std::pair{Country::kCuba, "Cuba"},
    std::pair{Country::kCyprus, "Cyprus"},
    std::pair{Country::kCzechRepublic, "Czech Republic"},
    std::pair{Country::kDenmark, "Denmark"},
    std::pair{Country::kDjibouti, "Djibouti"},
    std::pair{Country::kDominica, "Dominica"},
    std::pair{Country::kDominicanRepublic, "Dominican Republic"},
    std::pair{Country::kEastTimor, "East Timor"},
    std::pair{Country::kEcuador, "Ecuador"},
    std::pair{Country::kEgypt, "Egypt"},
    std::pair{Country::kElSalvador, "El Salvador"},
    std::pair{Country::kEquatorialGuinea, "Equatorial Guinea"},
    std::pair{Country::kEritrea, "Eritrea"},
    std::pair{Country::kEstonia, "Estonia"},
    std::pair{Country::kEthiopia, "Ethiopia"},
    std::pair{Country::kFiji, "Fiji"},
    std::pair{Country::kFinland, "Finland"},
    std::pair{Country::kFrance, "France"},
    std::pair{Country::kGabon, "Gabon"},
    std::pair{Country::kGambia, "Gambia"},
    std::pair{Country::kGeorgia, "Georgia"},
    std::pair{Country::kGermany, "Germany"},
    std::pair{Country::kGhana, "Ghana"},
    std::pair{Country::kGreece, "Greece"},
    std::pair{Country::kGrenada, "Grenada"},
    std::pair{Country::kGuatemala, "Guatemala"},
    std::pair{Country::kGuinea, "Guinea"},
    std::pair{Country::kGuineaBissau, "Guinea-Bissau"},
    std::pair{Country::kGuyana, "Guyana"},
    std::pair{Country::kHaiti, "Haiti"},
    std::pair{Country::kHonduras, "Honduras"},
    std::pair{Country::kHungary, "Hungary"},
    std::pair{Country::kIceland, "Iceland"},
    std::pair{Country::kIndia, "India"},
    std::pair{Country::kIndonesia, "Indonesia"},
    std::pair{Country::kIran, "Iran"},
    std::pair{Country::kIraq, "Iraq"},
    std::pair{Country::kIrelandRepublic, "Republic of Ireland"},
    std::pair{Country::kIsrael, "Israel"},
    std::pair{Country::kItaly, "Italy"},
    std::pair{Country::kIvoryCoast, "Ivory Coast"},
    std::pair{Country::kJamaica, "Jamaica"},
    std::pair{Country::kJapan, "Japan"},
    std::pair{Country::kJordan, "Jordan"},
    std::pair{Country::kKazakhstan, "Kazakhstan"},
    std::pair{Country::kKenya, "Kenya"},
    std::pair{Country::kKiribati, "Kiribati"},
    std::pair{Country::kKoreaNorth, "Korea North"},
    std::pair{Country::kKoreaSouth, "Korea South"},
    std::pair{Country::kKosovo, "Kosovo"},
    std::pair{Country::kKuwait, "Kuwait"},
    std::pair{Country::kKyrgyzstan, "Kyrgyzstan"},
    std::pair{Country::kLaos, "Laos"},
    std::pair{Country::kLatvia, "Latvia"},
    std::pair{Country::kLebanon, "Lebanon"},
    std::pair{Country::kLesotho, "Lesotho"},
    std::pair{Country::kLiberia, "Liberia"},
    std::pair{Country::kLibya, "Libya"},
    std::pair{Country::kLiechtenstein, "Liechtenstein"},
    std::pair{Country::kLithuania, "Lithuania"},
    std::pair{Country::kLuxembourg, "Luxembourg"},
    std::pair{Country::kMacedonia, "Macedonia"},
    std::pair{Country::kMadagascar, "Madagascar"},
    std::pair{Country::kMalawi, "Malawi"},
    std::pair{Country::kMalaysia, "Malaysia"},
    std::pair{Country::kMaldives, "Maldives"},
    std::pair{Country::kMali, "Mali"},
    std::pair{Country::kMalta, "Malta"},
    std::pair{Country::kMarshallIslands, "Marshall Islands"},
    std::pair{Country::kMauritania, "Mauritania"},
    std::pair{Country::kMauritius, "Mauritius"},
    std::pair{Country::kMexico, "Mexico"},
    std::pair{Country::kMicronesia, "Micronesia"},
    std::pair{Country::kMoldova, "Moldova"},
    std::pair{Country::kMonaco, "Monaco"},
    std::pair{Country::kMongolia, "Mongolia"},
    std::pair{Country::kMontenegro, "Montenegro"},
    std::pair{Country::kMorocco, "Morocco"},
    std::pair{Country::kMozambique, "Mozambique"},
    std::pair{Country::kMyanmarBurma, "Myanmar, Burma"},
    std::pair{Country::kNamibia, "Namibia"},
    std::pair{Country::kNauru, "Nauru"},
    std::pair{Country::kNepal, "Nepal"},
    std::pair{Country::kNetherlands, "Netherlands"},
    std::pair{Country::kNewZealand, "New Zealand"},
    std::pair{Country::kNicaragua, "Nicaragua"},
    std::pair{Country::kNiger, "Niger"},
    std::pair{Country::kNigeria, "Nigeria"},
    std::pair{Country::kNorway, "Norway"},
    std::pair{Country::kOman, "Oman"},
    std::pair{Country::kPakistan, "Pakistan"},
    std::pair{Country::kPalau, "Palau"},
    std::pair{Country::kPanama, "Panama"},
    std::pair{Country::kPapuaNewGuinea, "Papua New Guinea"},
    std::pair{Country::kParaguay, "Paraguay"},
    std::pair{Country::kPeru, "Peru"},
    std::pair{Country::kPhilippines, "Philippines"},
    std::pair{Country::kPoland, "Poland"},
    std::pair{Country::kPortugal, "Portugal"},
    std::pair{Country::kQatar, "Qatar"},
    std::pair{Country::kRomania, "Romania"},
    std::pair{Country::kRussianFederation, "Russian Federation"},
    std::pair{Country::kRwanda, "Rwanda"},
    std::pair{Country::kStKittsAndNevis, "St Kitts & Nevis"},
    std::pair{Country::kStLucia, "St Lucia"},
    std::pair{Country::kSaintVincentAndTheGrenadines,
              "Saint Vincent & the Grenadines"},
    std::pair{Country::kSamoa, "Samoa"},
    std::pair{Country::kSanMarino, "San Marino"},
    std::pair{Country::kSaoTomeAndPrincipe, "Sao Tome & Principe"},
    std::pair{Country::kSaudiArabia, "Saudi Arabia"},
    std::pair{Country::kSenegal, "Senegal"},
    std::pair{Country::kSerbia, "Serbia"},
    std::pair{Country::kSeychelles, "Seychelles"},
    std::pair{Country::kSierraLeone, "Sierra Leone"},
    std::pair{Country::kSingapore, "Singapore"},
    std::pair{Country::kSlovakia, "Slovakia"},
    std::pair{Country::kSlovenia, "Slovenia"},
    std::pair{Country::kSolomonIslands, "Solomon Islands"},
    std::pair{Country::kSomalia, "Somalia"},
    std::pair{Country::kSouthAfrica, "South Africa"},
    std::pair{Country::kSouthSudan, "South Sudan"},
    std::pair{Country::kSpain, "Spain"},
    std::pair{Country::kSriLanka, "Sri Lanka"},
    std::pair{Country::kSudan, "Sudan"},
    std::pair{Country::kSuriname, "Suriname"},
    std::pair{Country::kSwaziland, "Swaziland"},
    std::pair{Country::kSweden, "Sweden"},
    std::pair{Country::kSwitzerland, "Switzerland"},
    std::pair{Country::kSyria, "Syria"},
    std::pair{Country::kTaiwan, "Taiwan"},
    std::pair{Country::kTajikistan, "Tajikistan"},
    std::pair{Country::kTanzania, "Tanzania"},
    std::pair{Country::kThailand, "Thailand"},
    std::pair{Country::kTogo, "Togo"},
    std::pair{Country::kTonga, "Tonga"},
    std::pair{Country::kTrinidadAndTobago, "Trinidad & Tobago"},
    std::pair{Country::kTunisia, "Tunisia"},
    std::pair{Country::kTurkey, "Turkey"},
    std::pair{Country::kTurkmenistan, "Turkmenistan"},
    std::pair{Country::kTuvalu, "Tuvalu"},
    std::pair{Country::kUganda, "Uganda"},
    std::pair{Country::kUkraine, "Ukraine"},
    std::pair{Country::kUnitedArabEmirates, "United Arab Emirates"},
    std::pair{Country::kUnitedKingdom, "United Kingdom"},
    std::pair{Country::kUnitedStates, "United States"},
    std::pair{Country::kUruguay, "Uruguay"},
    std::pair{Country::kUzbekistan, "Uzbekistan"},
    std::pair{Country::kVanuatu, "Vanuatu"},
    std::pair{Country::kVaticanCity, "Vatican City"},
    std::pair{Country::kVenezuela, "Venezuela"},
    std::pair{Country::kVietnam, "Vietnam"},
    std::pair{Country::kYemen, "Yemen"},
    std::pair{Country::kZambia, "Zambia"},
    std::pair{Country::kZimbabwe, "Zimbabwe"},
};

enum class Gender : int16_t {
  kMale = 0,
  kFemale = 1,
};

constexpr auto kAllGenders = std::array{
    std::pair{Gender::kMale, "Male"},
    std::pair{Gender::kFemale, "Female"},
};

struct User {
  user_id_t id;
  std::string email;
  std::string first_name;
  std::string last_name;
  int64_t created_at;

  MSGPACK_DEFINE(id, email, first_name, last_name, created_at);
};

struct Hotel {
  user_id_t id;
  std::string name;
  user_id_t owner_id;
  int64_t created_at;

  MSGPACK_DEFINE(id, name, owner_id, created_at);
};

enum class DocumentType : int16_t {
  kPassport,
};

constexpr auto kAllDocumentTypes = std::array{
    std::pair{DocumentType::kPassport, "Passport"},
};

struct Guest {
  guest_id_t id;
  std::string first_name;
  std::string last_name;
  int64_t date_of_birth;
  Gender gender;
  std::string phone_number;
  std::string email;
  DocumentType document_type;
  std::string document_number;
  Country document_country;
  int64_t document_valid_until;
  std::string notes;
  hotel_id_t hotel_id;
  int64_t created_at;
};

struct Room {
  room_id_t id;
  std::string number;
  std::string group_name;
  hotel_id_t hotel_id;
  int64_t created_at;

  MSGPACK_DEFINE(id, number, group_name, hotel_id, created_at);
};

struct Booking {
  booking_id_t id;
  hotel_id_t hotel_id;
  room_id_t room_id;
  int64_t check_in_at;
  int64_t check_out_at;
  int64_t created_at;
  std::vector<guest_id_t> guest_ids;

  MSGPACK_DEFINE(id, hotel_id, room_id, check_in_at, check_out_at, created_at,
                 guest_ids);
};

struct LongTimeGuest {
  guest_id_t guest_id;
  hotel_id_t hotel_id;
  room_id_t room_id;
  int64_t check_in_at;
  int64_t check_out_at;

  MSGPACK_DEFINE(guest_id, hotel_id, room_id, check_in_at, check_out_at);
};
}  // namespace entities

#endif  // DESKTOP_CLIENT_ENTITIES_H
