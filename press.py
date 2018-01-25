#!/usr/bin/env python3

from splinter import Browser
import sys
import time

browser = None

for profile in sys.argv[1:]:
	browser = Browser("firefox", profile="/home/alexander/.mozilla/firefox/" + profile, headless=True)
	url = "https://www.chrono.gg/"
	browser.visit(url)
	button = browser.find_by_id("reward-coin")
	button.click()
	time.sleep(2)
	browser.quit()
	
