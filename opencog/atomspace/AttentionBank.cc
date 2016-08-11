/*
 * opencog/atomspace/AttentionBank.cc
 *
 * Copyright (C) 2013 Linas Vepstas <linasvepstas@gmail.com>
 * All Rights Reserved
 *
 * Written by Joel Pitt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <boost/bind.hpp>

#include <opencog/atoms/base/Handle.h>
#include "AttentionBank.h"
#include "AtomTable.h"

#include <opencog/util/Config.h>

using namespace opencog;

AttentionBank::AttentionBank(AtomTable& atab, bool transient)
{
    /* Do not boether with initialization, if this is transient */
    if (transient) { _zombie = true; return; }
    _zombie = false;

    startingFundsSTI = fundsSTI = config().get_int("STARTING_STI_FUNDS",100000);
    startingFundsLTI = fundsLTI = config().get_int("STARTING_LTI_FUNDS",100000);
    stiFundsBuffer = config().get_int("STI_FUNDS_BUFFER",10000);
    ltiFundsBuffer = config().get_int("LTI_FUNDS_BUFFER",10000);
    targetLTI = config().get_int("TARGET_LTI_FUNDS",10000);
    targetSTI = config().get_int("TARGET_STI_FUNDS",10000);
    STIAtomWage = config().get_int("ECAN_STARTING_ATOM_STI_WAGE",10);
    LTIAtomWage = config().get_int("ECAN_STARTING_ATOM_LTI_WAGE",10);

    attentionalFocusBoundary = 1;

    AVChangedConnection =
        atab.AVChangedSignal().connect(
            boost::bind(&AttentionBank::AVChanged, this, _1, _2, _3));
}

/// This must be called before the AtomTable is destroyed. Which
/// means that it cannot be in the destructor (since the AtomTable
/// is probably gone by then, leading to a crash.  XXX FIXME yes this
/// is a tacky hack to fix a design bug.
void AttentionBank::shutdown(void)
{
    if (_zombie) return;  /* no-op, if a zombie */
    AVChangedConnection.disconnect();
}

AttentionBank::~AttentionBank() {}


void AttentionBank::AVChanged(Handle h, AttentionValuePtr old_av,
                                        AttentionValuePtr new_av)
{
    // Add the old attention values to the AtomSpace funds and
    // subtract the new attention values from the AtomSpace funds
    updateSTIFunds(old_av->getSTI() - new_av->getSTI());
    updateLTIFunds(old_av->getLTI() - new_av->getLTI());

    logger().fine("AVChanged: fundsSTI = %d, old_av: %d, new_av: %d",
                   fundsSTI, old_av->getSTI(), new_av->getSTI());

    // Check if the atom crossed into or out of the AttentionalFocus
    // and notify any interested parties
    if (old_av->getSTI() < attentionalFocusBoundary and
        new_av->getSTI() >= attentionalFocusBoundary)
    {
        AFCHSigl& afch = AddAFSignal();
        afch(h, old_av, new_av);
    }
    else if (new_av->getSTI() < attentionalFocusBoundary and
             old_av->getSTI() >= attentionalFocusBoundary)
    {
        AFCHSigl& afch = RemoveAFSignal();
        afch(h, old_av, new_av);
    }
}

long AttentionBank::getTotalSTI() const
{
    std::lock_guard<std::mutex> lock(lock_funds);
    return startingFundsSTI - fundsSTI;
}

long AttentionBank::getTotalLTI() const
{
    std::lock_guard<std::mutex> lock(lock_funds);
    return startingFundsLTI - fundsLTI;
}

long AttentionBank::getSTIFunds() const
{
    std::lock_guard<std::mutex> lock(lock_funds);
    return fundsSTI;
}

long AttentionBank::getLTIFunds() const
{
    std::lock_guard<std::mutex> lock(lock_funds);
    return fundsLTI;
}

long AttentionBank::updateSTIFunds(AttentionValue::sti_t diff)
{
    std::lock_guard<std::mutex> lock(lock_funds);
    fundsSTI += diff;
    return fundsSTI;
}

long AttentionBank::updateLTIFunds(AttentionValue::lti_t diff)
{
    std::lock_guard<std::mutex> lock(lock_funds);
    fundsLTI += diff;
    return fundsLTI;
}

void AttentionBank::updateMaxSTI(AttentionValue::sti_t m)
{
    std::lock_guard<std::mutex> lock(lock_maxSTI);
    maxSTI.update(m);
}

void AttentionBank::updateMinSTI(AttentionValue::sti_t m)
{
    std::lock_guard<std::mutex> lock(lock_minSTI);
    minSTI.update(m);
}

AttentionValue::sti_t AttentionBank::getMaxSTI(bool average) const
{
    std::lock_guard<std::mutex> lock(lock_maxSTI);
    if (average) {
        return (AttentionValue::sti_t) maxSTI.recent;
    } else {
        return maxSTI.val;
    }
}

AttentionValue::sti_t AttentionBank::getMinSTI(bool average) const
{
    std::lock_guard<std::mutex> lock(lock_minSTI);
    if (average) {
        return (AttentionValue::sti_t) minSTI.recent;
    } else {
        return minSTI.val;
    }
}

AttentionValue::sti_t AttentionBank::calculateSTIWage()
{
    long funds = getSTIFunds();
    float diff  = funds - targetSTI;
    float ndiff = diff / stiFundsBuffer;
    ndiff = std::min(ndiff,1.0f);
    ndiff = std::max(ndiff,-1.0f);

    return STIAtomWage + (STIAtomWage * ndiff);
}

AttentionValue::lti_t AttentionBank::calculateLTIWage()
{
    long funds = getLTIFunds();
    float diff  = funds - targetLTI;
    float ndiff = diff / ltiFundsBuffer;
    ndiff = std::min(ndiff,1.0f);
    ndiff = std::max(ndiff,-1.0f);

    return LTIAtomWage + (LTIAtomWage * ndiff);
}

AttentionValue::sti_t AttentionBank::getAttentionalFocusBoundary() const
{
    return attentionalFocusBoundary;
}

AttentionValue::sti_t AttentionBank::setAttentionalFocusBoundary(AttentionValue::sti_t boundary)
{
    attentionalFocusBoundary = boundary;
    return boundary;
}

float AttentionBank::getNormalisedSTI(AttentionValuePtr av, bool average, bool clip) const
{
    // get normalizer (maxSTI - attention boundary)
    int normaliser;
    float val;
    AttentionValue::sti_t s = av->getSTI();
    if (s > getAttentionalFocusBoundary()) {
        normaliser = (int) getMaxSTI(average) - getAttentionalFocusBoundary();
        if (normaliser == 0) {
            return 0.0f;
        }
        val = (s - getAttentionalFocusBoundary()) / (float) normaliser;
    } else {
        normaliser = -((int) getMinSTI(average) + getAttentionalFocusBoundary());
        if (normaliser == 0) {
            return 0.0f;
        }
        val = (s + getAttentionalFocusBoundary()) / (float) normaliser;
    }
    if (clip) {
        return std::max(-1.0f, std::min(val,1.0f));
    } else {
        return val;
    }
}

float AttentionBank::getNormalisedSTI(AttentionValuePtr av) const
{
    AttentionValue::sti_t s = av->getSTI();
    auto normaliser =
            s > getAttentionalFocusBoundary() ? getMaxSTI() : getMinSTI();

    return (s / normaliser);
}

float AttentionBank::getNormalisedZeroToOneSTI(AttentionValuePtr av, bool average, bool clip) const
{
    int normaliser;
    float val;
    AttentionValue::sti_t s = av->getSTI();
    normaliser = getMaxSTI(average) - getMinSTI(average);
    if (normaliser == 0) {
        return 0.0f;
    }
    val = (s - getMinSTI(average)) / (float) normaliser;
    if (clip) {
        return std::max(0.0f, std::min(val,1.0f));
    } else {
        return val;
    }
}
