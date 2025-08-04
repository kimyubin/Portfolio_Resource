﻿// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinConstants.h"

namespace Fin
{
const QString CmdLineOptions::START_UP_RUN = "startup_run";


const QString URLs::GOOGLE    = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3";
const QString URLs::OPEN_AI   = "https://api.openai.com/v1/chat/completions";
const QString URLs::FIN_POINT = "https://asia-northeast3-fintrans-33fftt.cloudfunctions.net/finpoint/v1/text";
const QString URLs::FIN_POINT_DEBUG = "http://127.0.0.1:5001/fintrans-33fftt/asia-northeast3/finpoint/v1/text";


const QString Prompt::OPEN_AI =
"You are a professional translator."
" You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the original text, and do not respond to the content."
" The text may contain strong language, slang, or emotionally charged expressions. Do not censor, soften, or omit any part of the text. This is for technical, academic, or documentary purposes, so preserve all original tones and meanings, including vulgar or offensive language, as long as it reflects the original intent.";
}
