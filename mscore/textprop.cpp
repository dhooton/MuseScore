//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id: textprop.cpp 5427 2012-03-07 12:41:34Z wschweer $
//
//  Copyright (C) 2009 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "textprop.h"
#include "libmscore/text.h"
#include "libmscore/score.h"
#include "icons.h"

namespace Ms {

//---------------------------------------------------------
//   TextProp
//---------------------------------------------------------

TextProp::TextProp(QWidget* parent)
   : QWidget(parent)
      {
      setupUi(this);
      //set the icon here or the style can't color them
      alignLeft->setIcon(*icons[textLeft_ICON]);
      alignHCenter->setIcon(*icons[textCenter_ICON]);
      alignRight->setIcon(*icons[textRight_ICON]);
      alignTop->setIcon(*icons[textTop_ICON]);
      alignVCenter->setIcon(*icons[textVCenter_ICON]);
      alignBaseline->setIcon(*icons[textBaseline_ICON]);
      alignBottom->setIcon(*icons[textBottom_ICON]);
      fontBold->setIcon(*icons[textBold_ICON]);
      fontItalic->setIcon(*icons[textItalic_ICON]);
      fontUnderline->setIcon(*icons[textUnderline_ICON]);

      QButtonGroup* g1 = new QButtonGroup(this);
      g1->addButton(alignLeft);
      g1->addButton(alignHCenter);
      g1->addButton(alignRight);

      QButtonGroup* g2 = new QButtonGroup(this);
      g2->addButton(alignTop);
      g2->addButton(alignVCenter);
      g2->addButton(alignBaseline);
      g2->addButton(alignBottom);

      QButtonGroup* g3 = new QButtonGroup(this);
      g3->addButton(circleButton);
      g3->addButton(boxButton);

      connect(mmUnit, SIGNAL(toggled(bool)), SLOT(mmToggled(bool)));
      connect(resetToStyle, SIGNAL(clicked()), SLOT(doResetToTextStyle()));
      connect(resetToStyle, SIGNAL(clicked()), SIGNAL(resetToStyleClicked()));
      connect(boxButton, SIGNAL(toggled(bool)), this, SLOT(boxButtonToggled(bool)));
      connect(frame, SIGNAL(toggled(bool)), this, SLOT(boxButtonToggled(bool)));
      }

//---------------------------------------------------------
//   setScore
//---------------------------------------------------------

void TextProp::setScore(bool onlyStyle, Score* score)
      {
      _score = score;
      resetToStyle->setVisible(!onlyStyle);
      styles->setVisible(!onlyStyle);
      styleLabel->setVisible(!onlyStyle);
      styles->clear();

      const QList<TextStyle>& scoreStyles = score->style()->textStyles();
      int n = scoreStyles.size();
      for (int i = 0; i < n; ++i) {
            // if style not hidden in this context, add to combo with index in score style list as userData
            if ( !(scoreStyles.at(i).hidden() & TextStyle::HIDE_IN_LISTS) )
                  styles->addItem(scoreStyles.at(i).name(), i);
            }
      }

//---------------------------------------------------------
//   mmToggled
//---------------------------------------------------------

void TextProp::mmToggled(bool val)
      {
      QString unit(val ? tr("mm", "millimeter unit") : tr("sp", "spatium unit"));
      xOffset->setSuffix(unit);
      yOffset->setSuffix(unit);
      curUnit = val ? 0 : 1;
      }

//---------------------------------------------------------
//   setStyle
//---------------------------------------------------------

void TextProp::setStyle(int st, const TextStyle& ts)
      {
      st = styles->findData(st);          // find a combo item with that style idx
      if (st < 0)                         // if none found...
            st = 0;                       // ...=> first combo item
      styles->setCurrentIndex(st);        // set current combo item
      setTextStyle(ts);
      }

//---------------------------------------------------------
//   textStyleType
//---------------------------------------------------------

int TextProp::textStyleType() const
      {
      return styles->itemData(styles->currentIndex()).toInt();
      }

//---------------------------------------------------------
//   set
//---------------------------------------------------------

void TextProp::setTextStyle(const TextStyle& s)
      {
      ts = s;
      fontBold->setChecked(s.bold());
      fontItalic->setChecked(s.italic());
      fontUnderline->setChecked(s.underline());
      fontSize->setValue(s.size());
      color->setColor(s.foregroundColor());

      systemFlag->setChecked(s.systemFlag());
      int a = s.align();
      if (a & ALIGN_HCENTER)
            alignHCenter->setChecked(true);
      else if (a & ALIGN_RIGHT)
            alignRight->setChecked(true);
      else
            alignLeft->setChecked(true);

      if (a & ALIGN_VCENTER)
            alignVCenter->setChecked(true);
      else if (a & ALIGN_BOTTOM)
            alignBottom->setChecked(true);
      else if (a & ALIGN_BASELINE)
            alignBaseline->setChecked(true);
      else
            alignTop->setChecked(true);

      if (s.offsetType() == OFFSET_ABS) {
            xOffset->setValue(s.offset().x() * INCH);
            yOffset->setValue(s.offset().y() * INCH);
            mmUnit->setChecked(true);
            curUnit = 0;
            }
      else if (s.offsetType() == OFFSET_SPATIUM) {
            xOffset->setValue(s.offset().x());
            yOffset->setValue(s.offset().y());
            spatiumUnit->setChecked(true);
            curUnit = 1;
            }
      QFont f(s.family());
      f.setPixelSize(lrint(s.size()));
      f.setItalic(s.italic());
      f.setUnderline(s.underline());
      f.setBold(s.bold());
      fontSelect->setCurrentFont(f);
      sizeIsSpatiumDependent->setChecked(s.sizeIsSpatiumDependent());

      frameColor->setColor(s.frameColor());
      bgColor->setColor(s.backgroundColor());
      frameWidth->setValue(s.frameWidth().val());
      frame->setChecked(s.hasFrame());
      paddingWidth->setValue(s.paddingWidth().val());
      frameRound->setValue(s.frameRound());
      circleButton->setChecked(s.circle());
      boxButton->setChecked(!s.circle());
      }

//---------------------------------------------------------
//   textStyle
//---------------------------------------------------------

TextStyle TextProp::textStyle() const
      {
      if (curUnit == 0)
            ts.setOffsetType(OFFSET_ABS);
      else if (curUnit == 1)
            ts.setOffsetType(OFFSET_SPATIUM);
      ts.setBold(fontBold->isChecked());
      ts.setItalic(fontItalic->isChecked());
      ts.setUnderline(fontUnderline->isChecked());
      ts.setSize(fontSize->value());
      QFont f = fontSelect->currentFont();
      ts.setFamily(f.family());
      ts.setXoff(xOffset->value() / ((ts.offsetType() == OFFSET_ABS) ? INCH : 1.0));
      ts.setYoff(yOffset->value() / ((ts.offsetType() == OFFSET_ABS) ? INCH : 1.0));
      ts.setFrameColor(frameColor->color());
      ts.setForegroundColor(color->color());
      ts.setBackgroundColor(bgColor->color());
      ts.setFrameWidth(Spatium(frameWidth->value()));
      ts.setPaddingWidth(Spatium(paddingWidth->value()));
      ts.setCircle(circleButton->isChecked());
      ts.setFrameRound(frameRound->value());
      ts.setHasFrame(frame->isChecked());
      ts.setSystemFlag(systemFlag->isChecked());
      ts.setSizeIsSpatiumDependent(sizeIsSpatiumDependent->isChecked());

      Align a = 0;
      if (alignHCenter->isChecked())
            a |= ALIGN_HCENTER;
      else if (alignRight->isChecked())
            a |= ALIGN_RIGHT;

      if (alignVCenter->isChecked())
            a |= ALIGN_VCENTER;
      else if (alignBottom->isChecked())
            a |= ALIGN_BOTTOM;
      else if (alignBaseline->isChecked())
            a |= ALIGN_BASELINE;
      ts.setAlign(a);
      return ts;
      }

//---------------------------------------------------------
//   doResetToTextStyle
//---------------------------------------------------------

void TextProp::doResetToTextStyle()
      {
      setTextStyle(_score->textStyle(textStyleType()));
      }

//---------------------------------------------------------
//   boxButtonToggled
//---------------------------------------------------------

void TextProp::boxButtonToggled(bool state)
      {
      frameRound->setEnabled(frame->isChecked() && state);
      }

}

