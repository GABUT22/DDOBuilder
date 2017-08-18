// BreakdownItemTurnUndeadLevel.cpp
//
#include "stdafx.h"
#include "BreakdownItemTurnUndeadLevel.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemTurnUndeadLevel::BreakdownItemTurnUndeadLevel(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
}

BreakdownItemTurnUndeadLevel::~BreakdownItemTurnUndeadLevel()
{
}

// required overrides
CString BreakdownItemTurnUndeadLevel::Title() const
{
    CString text("Turn Undead Level");
    return text;
}

CString BreakdownItemTurnUndeadLevel::Value() const
{
    CString value;

    value.Format(
            "%d",
            (int)Total());
    return value;
}

void BreakdownItemTurnUndeadLevel::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();

        // your base level is the higher off:
        // effective cleric level or effective paladin level - 3
        // get the caster level breakdowns for cleric and paladin
        BreakdownItem * pCB = m_pBreakdownView->FindBreakdown(Breakdown_CasterLevel_Cleric);
        pCB->AttachObserver(this);
        BreakdownItem * pPB = m_pBreakdownView->FindBreakdown(Breakdown_CasterLevel_Paladin);
        pPB->AttachObserver(this);
        double clericLevels = pCB->Total();
        double paladinLevels = pPB->Total() - 3;
        if (clericLevels >= paladinLevels)
        {
            ActiveEffect cl(
                    Bonus_class,
                    "Cleric Levels",
                    1,
                    clericLevels,
                    "");        // no tree
            AddOtherEffect(cl);
        }
        else
        {
            ActiveEffect pl(
                    Bonus_class,
                    "Effective Cleric Levels (Paladin-3)",
                    1,
                    paladinLevels,
                    "");        // no tree
            AddOtherEffect(pl);
        }
    }
}

bool BreakdownItemTurnUndeadLevel::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies to this save
    bool isUs = false;
    if (effect.Type() == Effect_TurnLevelBonus)
    {
        isUs = true;
    }
    return isUs;
}

// BreakdownObserver overrides
void BreakdownItemTurnUndeadLevel::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // a class level that a base effect depends on has changed
    CreateOtherEffects();
    Populate();
}

void BreakdownItemTurnUndeadLevel::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}
