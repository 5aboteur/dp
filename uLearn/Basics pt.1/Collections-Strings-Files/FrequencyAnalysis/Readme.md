Продолжайте работу в том же проекте.

Биграмма — это пара соседних слов предложения. Например, из текста: "She stood up. Then she left." можно выделить следующие биграммы "She stood", "stood up", "Then she" и "she left", но не "up then".

Составьте словарь, который для каждого слова W хранит вторую часть V, наиболее частотной биграммы (W, V), начинающейся с W. Такой словарь называется биграммной моделью текста.

Ключи этого словаря — это все слова, с которых начинается хотя бы одна биграмма исходного текста. В качестве значения должно быть второе слово самой частой биграммы, начинающейся с ключа.

Если есть несколько биграмм с одинаковой частотой, то использовать лексикографически первую (используйте способ сравнения Ordinal, например с помощью метода string.CompareOrdinal).

Реализуйте этот алгоритм в классе FrequencyAnalysisTask.

Проверяющая система сначала запустит эталонный способ разделения исходного текста на предложения и слова, а затем вызовет реализованный вами метод. В случае ошибки вы увидите исходный текст, на котором запускался процесс тестирования.