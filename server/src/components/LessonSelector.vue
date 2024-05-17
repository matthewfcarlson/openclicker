<script setup>
import { ref } from "vue";

import imageUrl from "../../src/assets/imgs/home.png";
import {useLessonStore} from "../store/lesson";
const errorMessage = ref("");
const lessonStore = useLessonStore();
const openFile = async () => {
  errorMessage.value = "";
  const fileContents = await window.electronAPI.openFile();
  const errors = lessonStore.attemptLoad(fileContents);
  if (errors != null) errorMessage.value = errors;
};

</script>
<template>
  <div style="background-color: beige">
    <h1>Lesson Plan</h1>
    <button type="button" @click="openFile()">Load Lesson Plan</button>
    {{ errorMessage }}
  </div>
</template>