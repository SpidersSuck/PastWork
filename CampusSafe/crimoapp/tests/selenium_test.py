from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
import time

driver = webdriver.Chrome()  

try:
    driver.get("http://127.0.0.1:8000/")
    time.sleep(1)

    #test1 for not holding button long enough  
    button = driver.find_element(By.ID, "emergency-button")
    actions = ActionChains(driver)

    print("Test 1: Hold Short Test Begins")
    actions.click_and_hold(button).pause(3).release().perform()
    time.sleep(2)

    message = driver.find_element(By.ID, "alert-message")
    assert "not held long enough" in message.text.lower()
    print("Test 1 Passed: Alert was cancel and message was display")

    time.sleep(2)

    #test 2 holding for enough time
    button = driver.find_element(By.ID, "emergency-button")
    actions.click_and_hold(button).pause(11).release().perform()
    print("Holding the emergency button")

    time.sleep(2)

    message = driver.find_element(By.ID, "alert-message")
    assert "help is on the way" in message.text.lower()
    print("Test 2 Passed: Alert was successfully trigger message was display")

    print("All test have passed succefully")

except Exception as e:
    print("Test failed", e)


driver.quit()