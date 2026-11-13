/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2016 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#include "KeyMapTests.h"

#include "deskflow/KeyMap.h"

using namespace deskflow;
using KeyItemList = KeyMap::KeyItemList;
using KeyEntryList = std::vector<KeyItemList>;

void KeyMapTests::findBestKey_requiredDown_matchExactFirstItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList;
  KeyMap::KeyItem item;
  item.m_required = KeyModifierShift;
  item.m_sensitive = KeyModifierShift;
  KeyModifierMask desiredState = KeyModifierShift;
  itemList.push_back(item);
  entryList.push_back(itemList);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 0);
}

void KeyMapTests::findBestKey_requiredAndExtraSensitiveDown_matchExactFirstItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList;
  KeyMap::KeyItem item;
  item.m_required = KeyModifierShift;
  item.m_sensitive = KeyModifierShift | KeyModifierAlt;
  KeyModifierMask desiredState = KeyModifierShift;
  itemList.push_back(item);
  entryList.push_back(itemList);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 0);
}

void KeyMapTests::findBestKey_requiredAndExtraSensitiveDown_matchExactSecondItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList1;
  KeyMap::KeyItem item1;
  item1.m_required = KeyModifierAlt;
  item1.m_sensitive = KeyModifierShift | KeyModifierAlt;
  KeyMap::KeyItemList itemList2;
  KeyMap::KeyItem item2;
  item2.m_required = KeyModifierShift;
  item2.m_sensitive = KeyModifierShift | KeyModifierAlt;
  KeyModifierMask desiredState = KeyModifierShift;
  itemList1.push_back(item1);
  itemList2.push_back(item2);
  entryList.push_back(itemList1);
  entryList.push_back(itemList2);
  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 1);
}

void KeyMapTests::findBestKey_extraSensitiveDown_matchExactSecondItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList1;
  KeyMap::KeyItem item1;
  item1.m_required = 0;
  item1.m_sensitive = KeyModifierAlt;
  KeyMap::KeyItemList itemList2;
  KeyMap::KeyItem item2;
  item2.m_required = 0;
  item2.m_sensitive = KeyModifierShift;
  KeyModifierMask desiredState = KeyModifierAlt;
  itemList1.push_back(item1);
  itemList2.push_back(item2);
  entryList.push_back(itemList1);
  entryList.push_back(itemList2);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 1);
}

void KeyMapTests::findBestKey_noRequiredDown_matchOneRequiredChangeItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList1;
  KeyMap::KeyItem item1;
  item1.m_required = KeyModifierShift | KeyModifierAlt;
  item1.m_sensitive = KeyModifierShift | KeyModifierAlt;
  KeyMap::KeyItemList itemList2;
  KeyMap::KeyItem item2;
  item2.m_required = KeyModifierShift;
  item2.m_sensitive = KeyModifierShift | KeyModifierAlt;
  KeyModifierMask desiredState = 0;
  itemList1.push_back(item1);
  itemList2.push_back(item2);
  entryList.push_back(itemList1);
  entryList.push_back(itemList2);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 1);
}

void KeyMapTests::findBestKey_onlyOneRequiredDown_matchTwoRequiredChangesItem()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList1;
  KeyMap::KeyItem item1;
  item1.m_required = KeyModifierShift | KeyModifierAlt | KeyModifierControl;
  item1.m_sensitive = KeyModifierShift | KeyModifierAlt | KeyModifierControl;
  KeyItemList itemList2;
  KeyMap::KeyItem item2;
  item2.m_required = KeyModifierShift | KeyModifierAlt;
  item2.m_sensitive = KeyModifierShift | KeyModifierAlt | KeyModifierControl;
  KeyModifierMask desiredState = 0;
  itemList1.push_back(item1);
  itemList2.push_back(item2);
  entryList.push_back(itemList1);
  entryList.push_back(itemList2);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), 1);
}

void KeyMapTests::findBestKey_noRequiredDown_cannotMatch()
{
  KeyMap keyMap;
  KeyEntryList entryList;
  KeyItemList itemList;
  KeyMap::KeyItem item;
  item.m_required = 0xffffffff;
  item.m_sensitive = 0xffffffff;
  KeyModifierMask desiredState = 0;
  itemList.push_back(item);
  entryList.push_back(itemList);

  QCOMPARE(keyMap.findBestKey(entryList, desiredState), -1);
}

void KeyMapTests::isCommand()
{
  KeyMap keyMap;
  KeyModifierMask mask = KeyModifierShift;
  QVERIFY(!keyMap.isCommand(mask));

  mask = KeyModifierControl;
  QVERIFY(keyMap.isCommand(mask));

  mask = KeyModifierAlt;
  QVERIFY(keyMap.isCommand(mask));

  mask = KeyModifierAltGr;
  QVERIFY(keyMap.isCommand(mask));

  mask = KeyModifierMeta;
  QVERIFY(keyMap.isCommand(mask));

  mask = KeyModifierSuper;
  QVERIFY(keyMap.isCommand(mask));
}

void KeyMapTests::mapkey()
{
  KeyMap keyMap{};
  KeyMap::Keystroke stroke('A', true, false, 1);
  KeyMap::KeyItem keyItem;
  keyItem.m_button = 'A';
  keyItem.m_group = 1;
  keyItem.m_id = 'A';
  keyMap.addKeyEntry(keyItem);
  keyMap.finish();
  KeyMap::Keystrokes strokes{stroke};
  KeyMap::ModifierToKeys activeModifiers{};
  KeyModifierMask currentState{};
  KeyModifierMask desiredMask{};
  auto result = keyMap.mapKey(strokes, kKeySetModifiers, 1, activeModifiers, currentState, desiredMask, false, "en");
  QVERIFY(result != nullptr);
  desiredMask = KeyModifierControl;
  result = keyMap.mapKey(strokes, kKeySetModifiers, 1, activeModifiers, currentState, desiredMask, false, "en");
  QVERIFY(result == nullptr);
}

void KeyMapTests::mapKey_languageSync_data()
{
  QTest::addColumn<int>("currentGroup");
  QTest::addColumn<QString>("language");
  QTest::addColumn<int>("expectedGroup");
  QTest::addColumn<bool>("repeat");

  QTest::newRow("english-to-chinese") << 1 << QString("zh") << 2 << false;
  QTest::newRow("chinese-to-english") << 2 << QString("en") << 1 << false;
  QTest::newRow("repeat-english-to-chinese") << 1 << QString("zh") << 2 << true;
  QTest::newRow("matching-duplicate-language") << 3 << QString("en") << 3 << false;
  QTest::newRow("missing-language") << 2 << QString("de") << 2 << false;
  QTest::newRow("unsupported-language") << 2 << QString() << 2 << false;
  QTest::newRow("unsupported-current-source") << 0 << QString("zh") << 2 << false;
}

void KeyMapTests::mapKey_languageSync()
{
  QFETCH(int, currentGroup);
  QFETCH(QString, language);
  QFETCH(int, expectedGroup);
  QFETCH(bool, repeat);

  KeyMap keyMap;
  // The first source has an unsupported language; it must retain its index.
  // All sources share the same physical ASCII mapping, including the IME.
  keyMap.setLanguageData({"", "en", "zh", "en"});
  for (int32_t group = 0; group < 4; ++group) {
    KeyMap::KeyItem item;
    item.m_id = 'a';
    item.m_button = 1;
    item.m_group = group;
    keyMap.addKeyEntry(item);
  }
  keyMap.finish();

  KeyMap::Keystrokes strokes;
  KeyMap::ModifierToKeys modifiers;
  KeyModifierMask state = 0;
  const auto item = keyMap.mapKey(strokes, 'a', currentGroup, modifiers, state, 0, repeat, language.toStdString());
  QVERIFY(item != nullptr);
  QCOMPARE(item->m_id, static_cast<KeyID>('a'));
  QCOMPARE(item->m_group, expectedGroup);
  QCOMPARE(item->m_button, static_cast<KeyButton>(1));

  const bool changesGroup = currentGroup != expectedGroup;
  QCOMPARE(strokes.size(), static_cast<size_t>(changesGroup ? 2 : 1));
  if (changesGroup) {
    const auto &selection = strokes.front();
    QCOMPARE(selection.m_type, KeyMap::Keystroke::KeyType::Group);
    QCOMPARE(selection.m_data.m_group.m_group, expectedGroup);
    QVERIFY(selection.m_data.m_group.m_absolute);
    QVERIFY(!selection.m_data.m_group.m_restore);
  }

  const auto &key = strokes.back();
  QCOMPARE(key.m_type, KeyMap::Keystroke::KeyType::Button);
  QCOMPARE(key.m_data.m_button.m_button, static_cast<KeyButton>(1));
  QVERIFY(key.m_data.m_button.m_press);
  QCOMPARE(key.m_data.m_button.m_repeat, repeat);
  QCOMPARE(state, static_cast<KeyModifierMask>(0));
  QVERIFY(modifiers.empty());
}

void KeyMapTests::mapKey_languageSync_bidirectional()
{
  KeyMap keyMap;
  keyMap.setLanguageData({"en", "zh"});
  for (int32_t group = 0; group < 2; ++group) {
    KeyMap::KeyItem item;
    item.m_id = 'a';
    item.m_button = 1;
    item.m_group = group;
    keyMap.addKeyEntry(item);
  }
  keyMap.finish();

  int32_t currentGroup = 0;
  KeyMap::ModifierToKeys modifiers;
  KeyModifierMask state = 0;
  for (const auto &language : {"zh", "en"}) {
    KeyMap::Keystrokes strokes;
    const auto item = keyMap.mapKey(strokes, 'a', currentGroup, modifiers, state, 0, false, language);
    QVERIFY(item != nullptr);
    QCOMPARE(strokes.size(), static_cast<size_t>(2));
    const int32_t expectedGroup = 1 - currentGroup;
    QCOMPARE(strokes.front().m_type, KeyMap::Keystroke::KeyType::Group);
    QCOMPARE(strokes.front().m_data.m_group.m_group, expectedGroup);
    QCOMPARE(item->m_group, expectedGroup);
    QCOMPARE(strokes.back().m_type, KeyMap::Keystroke::KeyType::Button);
    QCOMPARE(strokes.back().m_data.m_button.m_button, static_cast<KeyButton>(1));
    QVERIFY(strokes.back().m_data.m_button.m_press);
    currentGroup = expectedGroup;
  }
  QCOMPARE(currentGroup, 0);
}

void KeyMapTests::mapKey_languageSync_missingTargetKey_usesMappedGroup()
{
  KeyMap keyMap;
  keyMap.setLanguageData({"en", "zh", "de"});
  KeyMap::KeyItem item;
  item.m_id = 'a';
  item.m_button = 1;
  item.m_group = 0;
  keyMap.addKeyEntry(item);
  item.m_id = 'b';
  item.m_button = 2;
  item.m_group = 2;
  keyMap.addKeyEntry(item);
  keyMap.finish();

  KeyMap::Keystrokes strokes;
  KeyMap::ModifierToKeys modifiers;
  KeyModifierMask state = 0;
  const auto mapped = keyMap.mapKey(strokes, 'a', 2, modifiers, state, 0, false, "zh");
  QVERIFY(mapped != nullptr);
  QCOMPARE(mapped->m_group, 0);
  QCOMPARE(strokes.size(), static_cast<size_t>(2));
  QCOMPARE(strokes.front().m_type, KeyMap::Keystroke::KeyType::Group);
  QCOMPARE(strokes.front().m_data.m_group.m_group, 0);
  QCOMPARE(strokes.back().m_type, KeyMap::Keystroke::KeyType::Button);
  QCOMPARE(strokes.back().m_data.m_button.m_button, static_cast<KeyButton>(1));
  QVERIFY(strokes.back().m_data.m_button.m_press);
}

void KeyMapTests::mapKey_languageSync_refreshKeepsAlignedGroups()
{
  KeyMap keyMap;
  keyMap.setLanguageData({"en", "zh"});
  KeyMap::KeyItem item;
  item.m_id = 'a';
  item.m_button = 1;
  for (int32_t group = 0; group < 2; ++group) {
    item.m_group = group;
    keyMap.addKeyEntry(item);
  }
  keyMap.finish();

  KeyMap refreshed;
  refreshed.setLanguageData({"", "zh", "en", "en"});
  for (int32_t group = 0; group < 4; ++group) {
    item.m_group = group;
    refreshed.addKeyEntry(item);
  }
  refreshed.finish();
  keyMap.swap(refreshed);

  for (const int32_t currentGroup : {0, 3}) {
    KeyMap::Keystrokes strokes;
    KeyMap::ModifierToKeys modifiers;
    KeyModifierMask state = 0;
    const auto mapped = keyMap.mapKey(strokes, 'a', currentGroup, modifiers, state, 0, false, "en");
    QVERIFY(mapped != nullptr);
    const int32_t expectedGroup = currentGroup == 0 ? 2 : 3;
    QCOMPARE(mapped->m_group, expectedGroup);
    QCOMPARE(strokes.size(), static_cast<size_t>(currentGroup == 0 ? 2 : 1));
    if (currentGroup == 0) {
      QCOMPARE(strokes.front().m_type, KeyMap::Keystroke::KeyType::Group);
      QCOMPARE(strokes.front().m_data.m_group.m_group, expectedGroup);
    }
    QCOMPARE(strokes.back().m_type, KeyMap::Keystroke::KeyType::Button);
    QCOMPARE(strokes.back().m_data.m_button.m_button, static_cast<KeyButton>(1));
    QVERIFY(strokes.back().m_data.m_button.m_press);
  }
}

void KeyMapTests::parseModifiers_plusKey_keepsPlusAsKey()
{
  std::string keystroke = "Control+Shift++";
  KeyModifierMask mask = 0;

  QVERIFY(KeyMap::parseModifiers(keystroke, mask));
  QCOMPARE(mask, static_cast<KeyModifierMask>(KeyModifierControl | KeyModifierShift));
  QCOMPARE(keystroke, std::string("+"));
}

void KeyMapTests::parseKey_plusSymbol_parsesAsAsciiKey()
{
  KeyID key = kKeyNone;

  QVERIFY(KeyMap::parseKey("+", key));
  QCOMPARE(key, static_cast<KeyID>('+'));
}

QTEST_MAIN(KeyMapTests)
