#include "client.h"

QString Client::getStatutFidelite() const {
    if (total_olives_livrees > 8000) return "Premium";
    if (total_olives_livrees > 5000) return "Gold";
    if (total_olives_livrees > 2000) return "Silver";
    return "Bronze";
}

QString Client::getImportance() const {
    if (total_olives_livrees > 3000) return "Important";
    if (total_olives_livrees > 1500) return "Moyen";
    return "Standard";
}

double Client::getScoreFidelite() const {
    return total_olives_livrees / 100.0;
}
